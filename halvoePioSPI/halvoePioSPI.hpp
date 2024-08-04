#pragma once

#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include "halvoePioSPI.pio.hpp"

namespace halvoePioSPI
{
  static int g_dmaChannelIndex = -1;

  static inline void clear_dma_interrupt_request()
  {
    dma_irqn_acknowledge_channel(DMA_IRQ_1, g_dmaChannelIndex);
  }

  static inline void restart_dma_channel(uint8_t* out_buffer, size_t in_bufferSize)
  {
    dma_channel_set_write_addr(g_dmaChannelIndex, out_buffer, false);
    dma_channel_set_trans_count(g_dmaChannelIndex, in_bufferSize, true);
  }

  static inline void halvoe_spi_dma_init(PIO io_pio, uint in_stateMachine,
                                        uint8_t* out_dmaBuffer, size_t in_dmaBufferSize,
                                        irq_handler_t in_dmaInterruptHandler)
  {
    g_dmaChannelIndex = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(g_dmaChannelIndex);
    channel_config_set_read_increment(&config, false);
    channel_config_set_write_increment(&config, true);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_dreq(&config, pio_get_dreq(io_pio, in_stateMachine, false));

    dma_channel_configure(g_dmaChannelIndex, &config,
      out_dmaBuffer,                 // Destination pointer
      &io_pio->rxf[in_stateMachine], // Source pointer
      in_dmaBufferSize,              // Number of transfers
      true                           // Start immediately
    );

    if (in_dmaInterruptHandler != nullptr)
    {
      dma_irqn_set_channel_enabled(DMA_IRQ_1, g_dmaChannelIndex, true);
      irq_set_exclusive_handler(DMA_IRQ_1, in_dmaInterruptHandler);
      irq_set_enabled(DMA_IRQ_1, true);
    }
  }

  static inline void halvoe_spi_peripheral_program_init(PIO io_pio, uint in_stateMachine, uint in_programMemoryOffset, uint in_dataPin,
                                                        uint8_t* out_dmaBuffer, size_t in_dmaBufferSize, irq_handler_t in_dmaInterruptHandler)
  {
    halvoe_spi_dma_init(io_pio, in_stateMachine, out_dmaBuffer, in_dmaBufferSize, in_dmaInterruptHandler);
    pio_sm_config config = halvoe_spi_peripheral_program_get_default_config(in_programMemoryOffset);
    // Set the IN base pin to the provided `pin` parameter. This is the data pin (RX),
    // and the next-numbered GPIO is used as the clock pin (SCK).
    sm_config_set_in_pins(&config, in_dataPin);
    // Set the pin directions to input at the PIO
    pio_sm_set_consecutive_pindirs(io_pio, in_stateMachine, in_dataPin, 2, false);
    // Connect these GPIOs to this PIO block
    pio_gpio_init(io_pio, in_dataPin);     // RX
    pio_gpio_init(io_pio, in_dataPin + 1); // SCK
    // Shifting to left matches the customary MSB-first ordering of SPI.
    sm_config_set_in_shift(
        &config,
        false, // Shift-to-right = false (i.e. shift to left)
        true,  // Autopush enabled
        8      // Autopush threshold = 8
    );
    // We only receive, so disable the TX FIFO to make the RX FIFO deeper.
    sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_RX);
    // Load our configuration, and start the program from the beginning
    pio_sm_init(io_pio, in_stateMachine, in_programMemoryOffset, &config);
    pio_sm_set_enabled(io_pio, in_stateMachine, true);
  }

  uint setup(PIO io_pio, uint in_dataPin, uint8_t* out_dmaBuffer, size_t in_dmaBufferSize, irq_handler_t in_dmaInterruptHandler = nullptr)
  {
    uint programMemoryOffset = pio_add_program(io_pio, &halvoe_spi_peripheral_program);
    uint stateMachine = pio_claim_unused_sm(io_pio, true);
    halvoe_spi_peripheral_program_init(io_pio, stateMachine, programMemoryOffset, in_dataPin, out_dmaBuffer, in_dmaBufferSize, in_dmaInterruptHandler);
    return stateMachine;
  }
}
