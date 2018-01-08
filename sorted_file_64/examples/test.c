#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sort_file.h"
#include "sorting_tools.h"

const char* names[] = {
  "Yannis",
  "Christofos",
  "Sofia",
  "Marianna",
  "Vagelis",
  "Maria",
  "Iosif",
  "Dionisis",
  "Konstantina",
  "Theofilos"
};

const char* surnames[] = {
  "Ioannidis",
  "Svingos",
  "Karvounari",
  "Rezkalla",
  "Nikolopoulos",
  "Berreta",
  "Koronis",
  "Gaitanis",
  "Oikonomou",
  "Mailis"
};

const char* cities[] = {
  "Athens",
  "San Francisco",
  "Los Angeles",
  "Amsterdam",
  "London",
  "New York",
  "Tokyo",
  "Hong Kong",
  "Munich",
  "Miami"
};

int main() {
  int fd;
  heap * my_heap =create_heap(28, 1, F_ID);
  Record record;
  srand(12569874);
  int r;
  printf("Insert Entries\n");
  for (int id = 0; id < 27; ++id) {
    record.id = id;
    r = rand() % 10;
    memcpy(record.name, names[r], strlen(names[r]) + 1);
    r = rand() % 10;
    memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
    r = rand() % 10;
    memcpy(record.city, cities[r], strlen(cities[r]) + 1);
    printf("%d %s, %s\n", record.id, record.name, record.surname);
    heap_node node;
    node.record = &record;
    // memcpy(&(node), &record, sizeof(Record));
    node.block_num = id;
    add_heap(my_heap, &node);
    // free(node.record);
  }
    // for(int i = 0 ; i < 5 ;i++){
      heap_node * node = pop_heap(my_heap);
      printf("node %s %s %d\n", node->record->name, node->record->surname, node->record->id);
    //   update_heap(my_heap, node);
    // }
  delete_heap(&my_heap);
}
