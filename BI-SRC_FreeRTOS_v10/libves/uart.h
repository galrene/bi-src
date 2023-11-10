/*************************************************************
 * COURSE WARE ver. 2.0
 * 
 * Permitted to use for educational and research purposes only.
 * NO WARRANTY.
 *
 * Faculty of Information Technology
 * Czech Technical University in Prague
 * Author: Miroslav Skrbek (C)2010,2011,2012
 *         skrbek@fit.cvut.cz
 * 
 **************************************************************
 */
#ifndef __UART_H
#define __UART_H

#define UART_DEFAULT_BAUDRATE  9600
#define UART_INT_ENABLE  1
#define UART_INT_DISABLE 0

typedef void (*uart_int_handler_t)(void);

extern void uart_init();
extern void uart_print_info();

extern void uart_write(unsigned char c);
extern unsigned char uart_read();
extern int uart_int_tx(int enable);
extern int uart_int_rx(int enable);
extern int uart_txbuf_is_full();
extern int uart_rxbuf_is_empty();
extern void uart_int_tx_clear();
extern void uart_int_rx_clear();
extern void uart_set_baud_rate(long rate);
extern void uart_set_tx_handler(uart_int_handler_t handler);
extern void uart_set_rx_handler(uart_int_handler_t handler);
extern int uart_is_rx_int_enabled();
extern int uart_is_tx_int_enabled();

#endif
