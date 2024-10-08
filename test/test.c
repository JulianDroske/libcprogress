#include "stdio.h"

#include "../cprogress.h"

#define CPROGRESS_IMPL
#include "../cprogress.h"

#define JURT_ENABLE_THREAD
#define JURT_IMPLEMENTATION
#include "jurt.h"


/* test interal */


void print_displaychunks(cprogress_t *cprogress) {
  printf("there are %d displaychunks.\n", cprogress->displaychunks_length);
  cprogress_displaychunk_foreach(cprogress, displaychunk) {
    switch (displaychunk->type) {
      default:
        printf("unknown displaychunk type %d\n", displaychunk->type);
        break;
      case CPROGRESS_DISPLAYCHUNK_TITLE:
        puts("type: title");
        break;
      case CPROGRESS_DISPLAYCHUNK_LITERAL:
        puts("type: literal");
        printf("literal: [%.*s]\n", displaychunk->literal_length, displaychunk->literal);
        break;
      case CPROGRESS_DISPLAYCHUNK_BAR:
        puts("type: progress bar");
        printf("fill char: %c\n", displaychunk->fill_char);
        break;
      case CPROGRESS_DISPLAYCHUNK_PERCENTAGE:
        puts("type: percentage");
        break;
    }

    if (displaychunk->is_autospan) {
      puts("this chunk is auto spanned");
    } else if (displaychunk->span_width != CPROGRESS_UNDEF) {
      printf("this chunk is adjusted to the length of %d chars\n", displaychunk->span_width);
    }

    puts("");
  }
}


int test_internal() {
  cprogress_t cprogress = cprogress_create("$=t [$20b#] $2p%%%", 1);
  if (cprogress.error) {
    printf("error occured with code %d\n", cprogress.error);
    return 1;
  }

  print_displaychunks(&cprogress);

  char buf[81];
  cprogress_writeline(&cprogress, buf, 80, 80, "HELLO", 40);
  printf("result: %s\n", buf);

  puts("as chars:");
  for (int i = 0; i < 81; ++i) {
    char ch = buf[i];
    printf("%d(%c) ", ch, ch);
  }
  puts("");

  cprogress_destroy(&cprogress);

  return 0;
}



/* test basic usage */


int test_usage() {
  cprogress_t cprogress = cprogress_create("$=t [$40b#] $p%", 4);
  if (cprogress.error) {
    printf("error occured with code %d\n", cprogress.error);
    return 1;
  }

  /* the whole rendering section here can be replaced with cprogress_render_tillcomplete(&cprogress, 30); */
  while (cprogress_stillrunning(&cprogress)) {

    cprogress_beginrender(&cprogress);

    /* when updating data: you can do it in any task, any it's not necessary to update title every time */
    static float percentage = 0;
    for (int i = 0; i < 4; ++i) {
      cprogress_updatetask_title(&cprogress, i, "Simple task");
      cprogress_updatetask_percentage(&cprogress, i, percentage += 0.5);
    }

    cprogress_render(&cprogress);
    cprogress_waitfps(&cprogress, 30);

    cprogress_endrender(&cprogress);
  }

  cprogress_destroy(&cprogress);

  return 0;
}



/* demo */


typedef struct {
  cprogress_t *cprogress;
  int task_index;
} demo_taskdata_t;

void *demo_thread_updater(void *userdata) {
  demo_taskdata_t *td = (demo_taskdata_t *) userdata;
  cprogress_t *cprogress = td->cprogress;
  int task_index = td->task_index;

  float percentage = 0;

  while (1) {
    cprogress_updatetask_percentage(cprogress, task_index, percentage += 10 + task_index * 2);
    jl_millisleep(1000);
  }
}

void *demo_thread_push_updater_delayed(void *userdata) {
  demo_taskdata_t *td = (demo_taskdata_t *) userdata;
  cprogress_t *cprogress = td->cprogress;
  int task_index = td->task_index;

  jl_millisleep(7000);
  cprogress_starttask(cprogress, task_index);
  cprogress_updatetask_title(cprogress, task_index, "New task");
  jl_createthread(demo_thread_updater, userdata, 0);
}

void demo_ontaskstop(cprogress_t *cprogress, int task_index) {
  // printf("DEMO: task %d stopped\n", task_index);
}

int demo() {
  cprogress_t cprogress = cprogress_create("$=t [$40b#] $p%", 4);
  if (cprogress.error) {
    printf("error occured with code %d\n", cprogress.error);
    return 1;
  }
  // cprogress_startalltasks(&cprogress);

  cprogress_subscribeevent(&cprogress, CPROGRESS_EVENT_THREADSTOP, demo_ontaskstop);

  demo_taskdata_t taskdatas[4] = {};
  for (int i = 0; i < 3; ++i) {
    cprogress_starttask(&cprogress, i);

    taskdatas[i] = (demo_taskdata_t) { &cprogress, i };
    char title[256] = {};
    snprintf(title, 255, "Simple task %d", i);
    cprogress_updatetask_title(&cprogress, i, title);

    /* jl_createtask(task_function, userdata, do_not_detach_from_current_task) */
    jl_createthread(demo_thread_updater, &taskdatas[i], 0);
  }

  taskdatas[3] = (demo_taskdata_t) { &cprogress, 3 };
  jl_createthread(demo_thread_push_updater_delayed, &taskdatas[3], 0);

  cprogress_render_tillcomplete(&cprogress, 2);

  cprogress_destroy(&cprogress);

  return 0;
}



/* switcher */


int main(void) {

  // return test_internal();
  // return test_usage();
  return demo();

  // return 0;
}

