#ifndef LPSCI_H_
#define LPSCI_H_

#include <stdint.h>

void lpsci_init();
void lpsci_send(char *buff, size_t len);
void lpsci_send_vals(int16_t *vals, uint8_t len);

#endif /* LPSCI_H_ */
