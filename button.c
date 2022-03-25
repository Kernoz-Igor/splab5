#include "DigitalOut.h"
#include "PinNameAliases.h"
#include "PinNamesTypes.h"
#include "ThisThread.h"
#include "mbed.h"
#include "pinmap.h"
#include <cstdio>
#include "mbed.h"

DigitalOut oled1(LED1);
DigitalOut testled(PC_7);

typedef struct {
float voltage; /* результат измерения напряжения */
float current; /* результат измерения тока*/
uint32_t counter;
int aboba;
} message_t;
/*здесь используется объект класса MemoryPool, для определения и
управления пулом памяти фиксированного размера, информацию об этом
классе изучите в документации */
MemoryPool<message_t, 16> mpool;
Queue<message_t, 16> queue;
Thread thread;
/* поток отправитель */
void send_thread(void)
{
uint32_t i = 0;
while (true) {

testled = !testled;

i++; /* так как настоящие данные брать неоткуда здесь они
генерируются*/
message_t *message = mpool.alloc();
message->voltage = (i * 0.1) * 33;
message->current = (i * 0.1) * 11;
message->counter = i;
message->aboba = testled;


ThisThread::sleep_for(1s);


queue.put(message);
ThisThread::sleep_for(1000);
}
}

int main(void)
{
thread.start(callback(send_thread));
while (true) {
osEvent evt = queue.get();
if (evt.status == osEventMessage) {
message_t *message = (message_t *)evt.value.p;
printf("\nVoltage: %.2f V\n\r", message->voltage);
printf("Current: %.2f A\n\r", message->current);
printf("Number of cycles: %u\n\r", message->counter);
printf("DIOD: %u\n\r", message->aboba);
mpool.free(message);

oled1 = message->aboba;
testled = message->aboba;
}

//oled1 = !oled1; // Включение/выключение светодиода
ThisThread::sleep_for(1s);

}
}