#include "sender.h"
#include "serializejson.h"
#include "ArduinoJson/Document/StaticJsonDocument.hpp"
#include "hal/gpio_types.h"
#include <cstddef>
#include <cstring>

static QueueHandle_t rdySem;
static esp_err_t g_ret;
static spi_device_handle_t handle;

static spi_transaction_t t;

static spi_bus_config_t g_buscfg;
static spi_device_interface_config_t g_devcfg;
static gpio_config_t g_io_conf;

static constexpr size_t CAPACITY = JSON_OBJECT_SIZE(200);
static char g_sendbuf[CAPACITY + 1];
static char g_recbuf[CAPACITY + 1];

static void IRAM_ATTR gpio_handshake_isr_handler(void* arg)
{
    //Sometimes due to interference or ringing or something, we get two irqs after eachother. This is solved by
    //looking at the time between interrupts and refusing any interrupt too close to another one.
    static uint32_t lasthandshaketime_us;
    uint32_t currtime_us = esp_timer_get_time();
    uint32_t diff = currtime_us - lasthandshaketime_us;
    if (diff < 1000) {
        return; //ignore everything <1ms after an earlier irq
    }
    lasthandshaketime_us = currtime_us;

    //Give the semaphore.
    BaseType_t mustYield = false;
    xSemaphoreGiveFromISR(rdySem, &mustYield);
    if (mustYield) {
        portYIELD_FROM_ISR();
    }
}

void sender()
{
    g_ret=spi_bus_add_device(SENDER_HOST, &g_devcfg, &handle);
    memset(&t, 0, sizeof(t));

    //Create the semaphore.
    rdySem=xSemaphoreCreateBinary();

    //Assume the slave is ready for the first transmission: if the slave started up before us, we will not detect
    //positive edge on the handshake line.
    xSemaphoreGive(rdySem);
    {
        memset(g_sendbuf, 0, sizeof(g_sendbuf)); 
        memset(g_recbuf, 0, sizeof(g_recbuf)); 

        serializeJson(GetJsonDoc(), g_sendbuf, CAPACITYx);
        GetJsonDoc().clear();
        t.length=sizeof(g_sendbuf);
        t.tx_buffer=g_sendbuf;
        t.rx_buffer=g_recbuf;
        delay(1000);
        //Wait for slave to be ready for next byte before sending
        xSemaphoreTake(rdySem, portMAX_DELAY); //Wait until slave is ready
        g_ret=spi_device_transmit(handle, &t);
        //printf("Received: %s\n", g_recbuf);
    }

    //Never reached.
    g_ret=spi_bus_remove_device(handle);
    assert(g_ret==ESP_OK);
}


void ConfigureSPIbus() 
{
    g_buscfg= spi_bus_config_t {
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    //Configuration for the SPI device on the other side of the bus
    g_devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .mode=0,
        .duty_cycle_pos=128, 
        .cs_ena_posttrans=3,
        .clock_speed_hz=5000000,       //50% duty cycle
        .spics_io_num=GPIO_CS,
        .queue_size=3       
        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
    };

    //GPIO config for the handshake line.
    g_io_conf= gpio_config_t {
        .pin_bit_mask=(1<<GPIO_NUM_2),
        .mode=GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type=GPIO_INTR_POSEDGE
    };
    SetUpHandShakeInterrupt();
}

void SetUpHandShakeInterrupt()
{
    //Set up handshake line interrupt.
    gpio_config(&g_io_conf);
    gpio_install_isr_service(0);
    gpio_set_intr_type(GPIO_NUM_2, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(GPIO_NUM_2, gpio_handshake_isr_handler, NULL);

    //Initialize the SPI bus and add the device we want to send stuff to.
    g_ret=spi_bus_initialize(SENDER_HOST, &g_buscfg, SPI_DMA_CH_AUTO);
    assert(g_ret==ESP_OK);
}


