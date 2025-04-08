#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define SIZE 10
#define DELAY 400000

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    int size;
    int field[SIZE][SIZE];
    Point snake_coords[SIZE * SIZE];
    int snake_length;
} Field;

typedef struct {
    char name[50];
    int direction;
    Point coords[SIZE * SIZE];
    int length;
    Field *field;
} Snake;

void generate_field(Field *field) {
    field->size = SIZE;
    field->snake_length = 0;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            field->field[i][j] = 0;
        }
    }
}

void clear_field(Field *field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (field->field[i][j] == 1 || field->field[i][j] == 2) {
                field->field[i][j] = 0;
            }
        }
    }
}

void add_entity(Field *field) {
    while(1) {
        int i = rand() % SIZE;
        int j = rand() % SIZE;
        int is_on_snake = 0;
        
        for (int k = 0; k < field->snake_length; k++) {
            if (field->snake_coords[k].x == i && field->snake_coords[k].y == j) {
                is_on_snake = 1;
                break;
            }
        }
        
        if (!is_on_snake) {
            field->field[i][j] = 3;
            break;
        }
    }
}

void render(Field *field, WINDOW *win) {
    clear_field(field);
    
    // Render snake on the field
    for (int i = 0; i < field->snake_length; i++) {
        Point p = field->snake_coords[i];
        field->field[p.x][p.y] = 1;
    }
    
    // Mark head
    if (field->snake_length > 0) {
        Point head = field->snake_coords[field->snake_length - 1];
        field->field[head.x][head.y] = 2;
    }
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            switch (field->field[i][j]) {
                case 0: wprintw(win, " . "); break;
                case 1: wprintw(win, " * "); break;
                case 2: wprintw(win, " # "); break;
                case 3: wprintw(win, " @ "); break;
            }
        }
        wprintw(win, "\n");
    }
}

Point get_entity_pos(Field *field) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (field->field[i][j] == 3) {
                Point p = {i, j};
                return p;
            }
        }
    }
    Point p = {-1, -1};
    return p;
}

int is_snake_eat_entity(Field *field) {
    Point entity = get_entity_pos(field);
    if (field->snake_length == 0) return 0;
    
    Point head = field->snake_coords[field->snake_length - 1];
    return (entity.x == head.x && entity.y == head.y);
}

void init_snake(Snake *snake, const char *name) {
    strcpy(snake->name, name);
    snake->direction = KEY_RIGHT;
    snake->length = 4;
    snake->field = NULL;
    
    // Init basic coords
    for (int i = 0; i < 4; i++) {
        snake->coords[i].x = 0;
        snake->coords[i].y = i;
    }
}

void set_direction(Snake *snake, int ch) {
    // Check if wrong direction
    if (ch == KEY_LEFT && snake->direction == KEY_RIGHT) return;
    if (ch == KEY_RIGHT && snake->direction == KEY_LEFT) return;
    if (ch == KEY_UP && snake->direction == KEY_DOWN) return;
    if (ch == KEY_DOWN && snake->direction == KEY_UP) return;
    
    snake->direction = ch;
}

Point check_limit(Snake *snake, Point point) {
    // Check field limit
    if (point.x > SIZE - 1) point.x = 0;
    else if (point.x < 0) point.x = SIZE - 1;
    else if (point.y < 0) point.y = SIZE - 1;
    else if (point.y > SIZE - 1) point.y = 0;
    
    return point;
}

int is_alive(Snake *snake) {
    if (snake->length == 0) return 1;
    
    Point head = snake->coords[snake->length - 1];
    for (int i = 0; i < snake->length - 1; i++) {
        if (snake->coords[i].x == head.x && snake->coords[i].y == head.y) {
            return 0;
        }
    }
    return 1;
}

void level_up(Snake *snake) {
    if (snake->length < 2) return;
    
    // get last point direction
    Point a = snake->coords[0];
    Point b = snake->coords[1];
    Point tail = a;
    
    if (a.x < b.x) tail.x -= 1;
    else if (a.y < b.y) tail.y -= 1;
    else if (a.x > b.x) tail.x += 1;
    else if (a.y > b.y) tail.y += 1;
    
    tail = check_limit(snake, tail);
    
    // Shift all coords to make space for new tail
    for (int i = snake->length; i > 0; i--) {
        snake->coords[i] = snake->coords[i - 1];
    }
    
    snake->coords[0] = tail;
    snake->length++;
    
    // Update field snake coords
    if (snake->field) {
        for (int i = 0; i < snake->length; i++) {
            snake->field->snake_coords[i] = snake->coords[i];
        }
        snake->field->snake_length = snake->length;
    }
}

void snake_move(Snake *snake) {
    if (snake->length == 0) return;
    
    // Determine head coords
    Point head = snake->coords[snake->length - 1];
    
    // Calc new head coords
    switch (snake->direction) {
        case KEY_UP: head.x -= 1; break;
        case KEY_DOWN: head.x += 1; break;
        case KEY_RIGHT: head.y += 1; break;
        case KEY_LEFT: head.y -= 1; break;
    }
    
    // Check field limit
    head = check_limit(snake, head);
    
    // Remove tail and add new head
    for (int i = 0; i < snake->length - 1; i++) {
        snake->coords[i] = snake->coords[i + 1];
    }
    snake->coords[snake->length - 1] = head;
    
    // Update field snake coords
    if (snake->field) {
        for (int i = 0; i < snake->length; i++) {
            snake->field->snake_coords[i] = snake->coords[i];
        }
        snake->field->snake_length = snake->length;
    }
    
    if (!is_alive(snake)) {
        endwin();
        exit(0);
    }
    
    // check if snake eat an entity
    if (snake->field && is_snake_eat_entity(snake->field)) {
        beep();
        level_up(snake);
        add_entity(snake->field);
    }
}

void set_field(Snake *snake, Field *field) {
    snake->field = field;
    field->snake_length = snake->length;
    for (int i = 0; i < snake->length; i++) {
        field->snake_coords[i] = snake->coords[i];
    }
}

int main() {
    // Initialize ncurses
    WINDOW *win = initscr();
    keypad(win, TRUE);
    nodelay(win, TRUE);
    noecho();
    curs_set(0);
    
    srand(time(NULL));
    
    // Init field & snake
    Field field;
    generate_field(&field);
    
    Snake snake;
    init_snake(&snake, "Joe");
    set_field(&snake, &field);
    
    add_entity(&field);
    
    while(1) {
        // Get last pressed key
        int ch = wgetch(win);
        if (ch != ERR) {
            set_direction(&snake, ch);
        }
        
        // Move snake
        snake_move(&snake);
        
        // Render field
        werase(win);
        render(&field, win);
        wrefresh(win);
        
        usleep(DELAY);
    }
    
    endwin();
    return 0;
}