#include <array>
#include <iostream>

#include <SDL2/SDL.h>

constexpr auto DEBUG = true;
#define DEBUG_PRINT(str) if(DEBUG) std::cout << str << std::endl;

constexpr auto win_side = 300;

class Game
{
private:
    SDL_Event m_event;

    SDL_Window* m_w;
    SDL_Renderer* m_r;
    bool m_running = true;    

    enum cell_state {NOTHING=0, CROSS=6, CIRCLE=7};

    cell_state m_grid[3][3] = {
        {cell_state::NOTHING, cell_state::NOTHING, cell_state::NOTHING},
        {cell_state::NOTHING, cell_state::NOTHING, cell_state::NOTHING},
        {cell_state::NOTHING, cell_state::NOTHING, cell_state::NOTHING},
    };

    cell_state m_player = cell_state::CROSS;
    
    void render()
    {
        SDL_SetRenderDrawColor(m_r, 0, 0, 0, 255);
	    SDL_RenderClear(m_r);

        SDL_SetRenderDrawColor(m_r, 255, 255, 255, 0);
	    for(auto i=1; i<3;i++)
		{
            SDL_RenderDrawLine(m_r, win_side/3*i, 0, win_side/3*i, win_side);
            SDL_RenderDrawLine(m_r, 0, win_side/3*i, win_side, win_side/3*i);
        }

        for (auto y=0;y<3;y++)
        {
            for (auto x=0;x<3;x++)
            {
                switch(m_grid[x][y])
                {
                    case cell_state::CROSS:
                        SDL_SetRenderDrawColor(m_r, 0, 0, 255, 255);
                        SDL_RenderDrawLine(m_r, x*100, y*100, (x*100)+100, (y*100)+100);
                        SDL_RenderDrawLine(m_r, (x*100)+100, (y*100), (x*100), (y*100)+100);

                        break;
                    case cell_state::CIRCLE: // another cross, fuck circles
                        SDL_SetRenderDrawColor(m_r, 255, 0, 0, 255);
                        SDL_RenderDrawLine(m_r, x*100, y*100+50, x*100+100, y*100+50);
                        SDL_RenderDrawLine(m_r, x*100+50, y*100, x*100+50, y*100+100);
                        break;
                    default:
                        break;
                }
            }
        }

        SDL_RenderPresent(m_r); // some kind of defer would be nice in this case :') 
    }

    void click_cell()
    {
        if(m_grid[m_event.button.x/100][m_event.button.y/100] != cell_state::NOTHING)
            return;
        else    
            m_grid[m_event.button.x/100][m_event.button.y/100] = m_player;

        m_player = m_player == cell_state::CROSS ? cell_state::CIRCLE : cell_state::CROSS;

        check_endgame();
    }

    void reset() {
        for (auto y=0;y<3;y++)
            for (auto x=0;x<3;x++)
                m_grid[x][y] = cell_state::NOTHING;
    }

    void check_endgame() // ugly function
    {
        auto sum_row=0;
        auto sum_column=0;
        auto sum_diag1 = 0; 
        for (auto y=0;y<3;y++)
        {
            for (auto x=0;x<3;x++)
            {
                sum_row += (int)m_grid[x][y];
                sum_column += (int)m_grid[y][x];
            }
            sum_diag1 += (int)m_grid[y][y];
            auto sum_diag2 = m_grid[2][0] + m_grid[1][1] + m_grid[0][2];
            if(sum_row == 6*3 || sum_row == 7*3 || sum_column == 6*3 || sum_column == 7*3 || sum_diag1 == 6*3 || sum_diag1 == 7*3 || sum_diag2 == 6*3 || sum_diag2 == 7*3) 
            {
                reset(); return;
            }
            sum_row = 0; sum_column = 0;
        }


    }

public:
    Game()
    {
        if(SDL_Init(SDL_INIT_VIDEO) != 0) std::exit(-1);

        m_w = SDL_CreateWindow("Tic-Tac-Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_side, win_side, 0);
        m_r = SDL_CreateRenderer(m_w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
        DEBUG_PRINT("constructor called");
    }

    ~Game()
    {
        SDL_DestroyRenderer(m_r);
	    SDL_DestroyWindow(m_w);
        SDL_Quit();
        DEBUG_PRINT("destructor called");
    }
    
    void gameloop()
    {
        const auto fps = 60;
        const auto frame_delay = 1000 / fps;
        Uint32 frame_start;
        int frame_time;

        while(m_running)
        {
            frame_start = SDL_GetTicks();
            while(SDL_PollEvent(&m_event))
            {
                switch (m_event.type)
                {
                case SDL_KEYDOWN:
                    switch (m_event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        return;
                        break;

                    default:
                        break;
                    }
                    break;
                
                case SDL_MOUSEBUTTONDOWN:
                    click_cell();
                    break;

                case SDL_QUIT:
                    return;
                    break;

                default:
                    break;
                }
            }

            // rendering game logic my ass ...
            render();

            frame_time = SDL_GetTicks() - frame_start;
            if(frame_delay > frame_time) SDL_Delay((frame_delay - frame_time));
        }
    }
};

int main(int argc, char *argv[])
{
    Game().gameloop();
    return 0;
}
