#include <ncurses.h>
#include <iostream>
#include <iomanip>
#include <math.h>

#include "matrix.h"

#define ELEMENT_SPACING 2
#define SIDE_SPACING 9
#define UNDERLINE_COUNT 3

struct Vec2{
    int x{};
    int y{};

    Vec2(){};
    Vec2(int x, int y): x(x), y(y){}
    Vec2(const Vec2& other): x(other.x), y(other.y){}
};

class UI{
    private:
        int _m_height{};
        int _m_width{};
        bool editting_cell{};

        void _highlight_mode(){
            if(edit_mode){
                wmove(control_window, 1, 2);
                waddstr(control_window, "MODO MOVIMENTO  ");
                wmove(control_window, 1, 20);
                wattron(control_window, A_STANDOUT);
                waddstr(control_window, "[MODO EDICAO]");
                wattroff(control_window, A_STANDOUT);
            }else{
                wmove(control_window, 1, 20);
                waddstr(control_window, "MODO EDICAO  ");
                wmove(control_window, 1, 2);
                wattron(control_window, A_STANDOUT);
                waddstr(control_window, "[MODO MOVIMENTO]");
                wattroff(control_window, A_STANDOUT);
            }
            wrefresh(control_window);
        }


        inline void _update_m_dims(){
            getmaxyx(m_window, _m_height, _m_width);
        }

        inline void _add_underline(int i = UNDERLINE_COUNT){
            for (int j = 0; j < i; ++j) waddch(m_window, ACS_HLINE);
        } 

        void _draw_matrix_borders(Matrix<float> m, int y = 0, int x = 0){
            std::array<int, 2> sh = m.get_shape();
            _m_height = ELEMENT_SPACING*sh[0] + sh[0];
            _m_width = SIDE_SPACING*(sh[1]+1) + sh[1];

            m_window = newwin(_m_height+1, _m_width+1, y+2, x);
            wattron(m_window, A_ALTCHARSET);

            // Draw all corners
            wmove(m_window, 0, 0);
            waddch(m_window, ACS_ULCORNER);
            _add_underline();
            wmove(m_window, _m_height, 0);
            waddch(m_window, ACS_LLCORNER);
            _add_underline();
            wrefresh(m_window);
            wmove(m_window, _m_height, _m_width-UNDERLINE_COUNT);
             _add_underline();
            waddch(m_window, ACS_LRCORNER);
            wmove(m_window, 0, _m_width-UNDERLINE_COUNT);
             _add_underline();
            waddch(m_window, ACS_URCORNER);

            // Draw both vertical bars at the sides
            for(int i = 1; i< _m_height; ++i){
                wmove(m_window, i, 0);
                waddch(m_window, ACS_VLINE);
                wmove(m_window, i, _m_width);
                waddch(m_window, ACS_VLINE);
            }
            wattroff(m_window, A_ALTCHARSET);
        }

        void _draw_elements(Matrix<float> m){
                std::array<int, 2> sh = m.get_shape();
                _update_m_dims();
                
                int y_spacing = (int)std::ceil((((float)_m_height-2)/(float)sh[0])); 
                int x_spacing = (int)std::ceil((((float)_m_width-2)/(float)sh[1]));

                for(int i = 0; i < sh[0]; ++i){
                    for(int j = 0; j < sh[1]; j++){
                        wmove(m_window, (i)*y_spacing+1, (j)*x_spacing+2);
                        if(el_cursor.y == i && el_cursor.x == j) wattron(m_window, A_BOLD | A_UNDERLINE | COLOR_PAIR(2));
                        wprintw(m_window, "%.5g", m.data[i][j]);
                        wrefresh(m_window);
                        wattroff(m_window, A_BOLD | A_UNDERLINE | COLOR_PAIR(2));
                    }
                }   
        }

        void _draw_input_prompt(std::string s){
            wclear(input_window);
            _update_input_header();
            wmove(input_window, 0, 20);
            waddstr(input_window, s.c_str());
            wrefresh(input_window);
        }

        void _update_input_header(){
            wattron(input_window, A_STANDOUT);
            wmove(input_window, 0, 0);
            wprintw(input_window,  "INPUT (%d, %d)", el_cursor.x, el_cursor.y);
            wattroff(input_window, A_STANDOUT);
            _draw_elements(m);
            wrefresh(input_window);
        }

        void _inc_y_el_cursor(int inc = 1){
            if(inc < 0) _dec_y_el_cursor(-inc);
            else{
                el_cursor.y += inc;
                if(el_cursor.y >= m.shape[0]) el_cursor.y = 0;
            }
        }

        void _dec_y_el_cursor(int dec = 1){
            el_cursor.y -= dec;
            if(edit_mode){
                if(el_cursor.y < 0) el_cursor.y = m.shape[0] - 1;
            }else{
                on_button = true;
            }
        }

    public:
        bool edit_mode{};
        bool on_button{};

        // Different windows for the different functions
        WINDOW *header_window;
        WINDOW *control_window;
        WINDOW *m_window;
        WINDOW *warning_window;
        WINDOW *input_window;
        WINDOW *button_window;

        Vec2 el_cursor{}; // element-wise cursor
        Vec2 ch_cursor{}; // character-wise cursor
        std::string buffer{}; // Input number buffer
        Matrix<float> m; // Matrix

        // Default Constructor
        UI(): edit_mode(0), m{{2, 3}}{

            int maxX, maxY;
            getmaxyx(stdscr, maxY, maxX);

            // Draw initial header
            header_window = newwin(2, 50, 0, 0);
            wattron(header_window, A_BOLD | A_UNDERLINE);
            waddstr(header_window, "Programa de calculo de matrizes ----- GRUPO 2");
            wmove(header_window, 1, 0);
            waddstr(header_window, "Matriz de INPUT");
            wrefresh(header_window);

            // Draw control header
            control_window = newwin(3, maxX, maxY-3, 0);
            wattron(control_window, A_ALTCHARSET);
            wborder(control_window, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, 
                                    ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

            wattroff(control_window, A_ALTCHARSET);
            _highlight_mode();
            
            // Draw Warning window
            warning_window = newwin(1, maxX, maxY-2, 45);
            wattron(warning_window, COLOR_PAIR(1));
            wattron(warning_window, A_INVIS);

            draw_matrix(this->m);

            ch_cursor.x = 20;
            input_window = newwin(2, 40, _m_height+3, 0);
            wattron(input_window, A_INVIS);
            wmove(input_window, 0, 0);

            

        }

        void next_element(bool row = false, int inc = 1){
            if(row){
                _inc_y_el_cursor(inc);
            }else{
                el_cursor.x += inc;
                if(el_cursor.x >= m.shape[1]) {
                    el_cursor.x = 0;
                    _inc_y_el_cursor();
                }
                if(el_cursor.x < 0){
                    el_cursor.x = m.shape[1]-1;
                    _dec_y_el_cursor();
                }
            }

            if(editting_cell) _update_input_header();
            if(!edit_mode) _draw_elements(m);
        }

        inline void prev_element(bool row = false){
            next_element(row, -1);
        }

        void next_char(){
            
        }

        void prev_char(){

        }

        void enter_signal(){
            _update_m_dims();

            if(edit_mode){
                if(!editting_cell){
                    wattroff(input_window, A_INVIS);
                    _update_input_header();
                    editting_cell = true;
                    wclear(warning_window);
                    wrefresh(warning_window);
                }else{

                    editting_cell = false;
                    float cell_val{};
                    if(buffer.size() > 0){
                        try{
                            cell_val = std::stof(buffer);
                        }catch(std::invalid_argument const &e){
                            std::cout << e.what();
                            wmove(warning_window, 0, 0);
                            waddstr(warning_window, "IMPOSSIVEL CONVERTER TEXTO PARA NUMERO DECIMAL!");
                            wattroff(warning_window, A_INVIS);
                            wrefresh(warning_window);
                        }
                    }

                    m[el_cursor.y][el_cursor.x] = cell_val; // Matriz em (x,y) e igual a cast string 
                    buffer.erase();
                    draw_matrix(m);         // Como m foi atualizado, deve redesenhar
                    _draw_input_prompt(""); // So pra apagar a window
                    wclear(input_window);
                    wrefresh(input_window);
                }
            }
        }

        void mov_mode(){
            edit_mode = false;
            _highlight_mode();
        }

        void switch_mode(){
            edit_mode = !edit_mode;
            if(!edit_mode){
                wclear(input_window);
                wrefresh(input_window);
            }
            _highlight_mode();
        }

        void addch_atcursor(int ch_raw){
            if(edit_mode && editting_cell){
                ++ch_cursor.x;
                buffer.append(1, char(ch_raw));
                _draw_input_prompt(buffer);
            }
        }

        void rmch_atcursor(){
            if(edit_mode && editting_cell){ 
                --ch_cursor.x;
                if(buffer.size() > 0) buffer.pop_back();
                _draw_input_prompt(buffer);
            }
        }

        void draw_matrix(Matrix<float> m, int y = 0, int x = 0){
            _draw_matrix_borders(m, y, x);
            _draw_elements(m);
            wrefresh(m_window);
        }
};

int main(){

    // Ncurses config. 
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    raw();
    refresh();
    timeout(16);
    curs_set(0);

    // Warning color
    start_color();
    init_color(COLOR_GREEN, 0, 700, 200);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    UI ui{};

    int ch_raw = 0;
    bool quit = false;

    while(!quit){
        
        ch_raw = getch();

        // Quitting
        if(ch_raw == 'q') quit = true;

        // Changing mode
        else if(ch_raw == 'm') ui.switch_mode();
        else if(ch_raw == KEY_ENTER || ch_raw == 10) ui.enter_signal();
        else if(ch_raw == 27) ui.mov_mode();
        
        // Mov mode
        if(!ui.edit_mode){
            if(ch_raw == KEY_RIGHT || ch_raw == '\t') ui.next_element();
            else if (ch_raw == KEY_LEFT) ui.prev_element();
            else if(ch_raw == KEY_UP) ui.prev_element(true);
            else if(ch_raw == KEY_DOWN) ui.next_element(true);
        // Edit mode
        }else{
            if(ch_raw == '\t') ui.next_element(); // TAB
            if(ch_raw == KEY_BACKSPACE) ui.rmch_atcursor();
            else if(ch_raw == KEY_LEFT) ui.next_char();
            else if (ch_raw == KEY_RIGHT) ui.prev_char();
            else if(ch_raw == KEY_UP) ui.prev_element(true);
            else if(ch_raw == KEY_DOWN) ui.next_element(true);
        }

        // If the char is 0-9 or the decimal point
        if(ui.edit_mode && ((ch_raw >=48 && ch_raw <= 57) || ch_raw == 46 || ch_raw == 45 ||
            ch_raw == 43)) ui.addch_atcursor(ch_raw);

        // CTRL Chars
        const char* c_ch = unctrl(ch_raw);
        if(c_ch[0] == '^'){
            if(c_ch[1] == 'X') quit = true;

            // COPIA COLA
        }
    }
    delwin(ui.header_window);
    delwin(ui.m_window);
    delwin(ui.control_window);
    endwin();

}