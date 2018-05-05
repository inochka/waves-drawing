#ifndef __QUEUE_C__

#define __QUEUE_C__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "prototypes.h"

// ************************************** Начало реализации очереди *********************************


QUEUE * create_queue() {
	QUEUE * q = (QUEUE *)malloc(sizeof(QUEUE));
	q->head = NULL;
	q->tail = NULL;
	return q;
}

int destroy_queue(QUEUE * q) {
	if (q) {
		QNODE * elem, *next;
		elem = q->head;
		// связь в списке - от хвоста к голове
		while (elem) {
			next = elem->next;
			free(elem);
			elem = next;
			// то есть когда указатель на следующий элемент - NULL,
			// останавливаемся.
		}
		free(q);
		return 0;
	}
	else {
		fprintf(stderr, "Попытка удаления несуществующей очереди!\n");
		exit(1);
	}
}

int enqueue(QUEUE * q, queueT a) {
	if (!q ) {
		fprintf(stderr, "Попытка пополнения несуществующей очереди!\n");
		exit(1);
	}
	QNODE *  new_elem = (QNODE *)malloc(sizeof(QNODE));
	new_elem->data = a;
	new_elem->next = NULL;
	// заносим в хвост, по поределению очереди
	if (q->tail && q->head) {
		(q->tail)->next = new_elem;
		q->tail = new_elem;
		// если не первый элемент
	}
	else {
		q->tail = new_elem;
		q->head = new_elem;
	}

	return 0;
}

queueT dequeue(QUEUE * q) {
	if (q && q->head) {
		QNODE * head_old = q->head;
		queueT a = head_old->data;
		q->head = head_old->next;
		// если в очереди единственый элемент - автоматически станет равно NULL,
		// как и у пустой очереди. А хвост - тоже нулл, так как free()
		// обнуляет соответсвующий указатель
		free(head_old);
		return a;
	}
	else {
		fprintf(stderr, "Попытка извлечь элемент из пустой или несуществующей очереди!\n");
		exit(1);
	}
}

void clear_queue(QUEUE * q) {
	if (q) {
		QNODE * elem, *next;
		elem = q->head;
		// связь в списке - от хвоста к голове
		while (elem) {
			next = elem->next;
			free(elem);
			elem = next;
			// то есть когда указатель на следующий элемент - NULL,
			// останавливаемся.
		}
		// отличается от delete только тем, что саму q в конце не удаляем
	}
	else {
		fprintf(stderr, "Попытка удаления несуществующей очереди!\n");
		exit(1);
	}
}

int is_empty_queue(QUEUE * q) {
	if (q) {
		if (q->tail && q->head)
			return 0;
		else
			return 1;
		// если фигня какая-то, то возвращаем, что пуста
	}
	else {
		fprintf(stderr, "Попытка обращения к несуществующей очереди!\n");
		exit(1);
	}
}



// *********************************** Конец реализации очереди ****************************************

#endif
