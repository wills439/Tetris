#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<memory>
#include<vector>
#include<random>

const int celllength = 50;
const int scrwidth = celllength*10;
const int scrheight = celllength*18;

struct Vec2INT{
    int x,y;
};

struct peice{
    int id;
    SDL_Rect src,dst;
    Vec2INT bufferPos;
    int rotationState;
    Vec2INT dimensions;
};

struct block
{
    int id;
    SDL_Rect src,dst;
};

int main()
{
    srand(time(0));

    int level=0; 
    int score=0;
    std::cout<<"Select Level[1-4]: ";
    std::cin>>level;
    while(level < 1 || level > 4){
        std::cout << "Invalid Input\n";
        std::cout<<"Select Level[1-4]";
        std::cin>>level;
    }
    
    //init sdl
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    
    auto window = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>(
        SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrwidth, scrheight, SDL_WINDOW_SHOWN),
        SDL_DestroyWindow
    );
    if(!window)
        std::cerr << "Window Error: " << SDL_GetError() << "\n";
    
    auto renderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>(
        SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED),
        SDL_DestroyRenderer
    );
    if(!renderer)
        std::cerr << "Renderer error: " << SDL_GetError() << "\n";

    auto spriteSheetIMG = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(
        IMG_Load("assets/tetrissprites.png"),
        SDL_FreeSurface
    );
    if(!spriteSheetIMG)
        std::cerr << "IMG Error: " << SDL_GetError() << "\n";

    auto spriteSheetTexture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(
        SDL_CreateTextureFromSurface(renderer.get(), spriteSheetIMG.get()),
        SDL_DestroyTexture
    );
    if(!spriteSheetTexture)
        std::cerr << "Texture Error: " << SDL_GetError() << "\n";

    //init entities and components
    std::vector<std::vector<SDL_Rect>> srcVectors;
    srcVectors.resize(7);
    
    std::vector<SDL_Rect> dsts;
    dsts.resize(7);

    std::vector<Vec2INT> dimensions;
    dimensions.resize(7);

    //list of list of 2d vectors
    std::vector<std::vector<std::vector<std::vector<int>>>> bufferVectors;
    bufferVectors.resize(7);

    std::vector<SDL_Rect> blockSrcs;
    blockSrcs.resize(7);

    //init peice srcs, dsts, intitial dimensions, and amount of buffer vectors
    int T = 0;
    srcVectors[T].resize(4);
    srcVectors[T][0] = {9,49,48,32};
    srcVectors[T][1] = {81,41,32,48};
    srcVectors[T][2] = {137,49,48,32};
    srcVectors[T][3] = {209,41,32,48};
    dimensions[T] = {3,2};
    bufferVectors[T].resize(4);
    blockSrcs[T] = {24,49,16,16};

    int orangel = 1;
    srcVectors[orangel].resize(4);
    srcVectors[orangel][0] = {9,113,48,32};
    srcVectors[orangel][1] = {81,105,32,48};
    srcVectors[orangel][2] = {137,113,48,32};
    srcVectors[orangel][3] = {209,105,32,48};
    dimensions[orangel] = {3,2};
    bufferVectors[orangel].resize(4);
    blockSrcs[orangel] = {9,128,16,16};

    int bluel = 2;
    srcVectors[bluel].resize(4);
    srcVectors[bluel][0] = {9,177,48,32};
    srcVectors[bluel][1] = {81,169,32,48};
    srcVectors[bluel][2] = {137,177,48,32};
    srcVectors[bluel][3] = {209,169,32,48};
    dimensions[bluel] = {3,2};
    bufferVectors[bluel].resize(4);
    blockSrcs[bluel] = {9,176,16,16};

    int pole = 3;
    srcVectors[pole].resize(2);
    srcVectors[pole][0] = {1,249,64,16};
    srcVectors[pole][1] = {89,225,16,64};
    dimensions[pole] = {4,1};
    bufferVectors[pole].resize(2);
    blockSrcs[pole] = {1,249,16,16};

    int reds = 4;
    srcVectors[reds].resize(2);
    srcVectors[reds][0] = {137,241,48,32};
    srcVectors[reds][1] = {209,233,32,48};
    dimensions[reds] = {3,2};
    bufferVectors[reds].resize(2);
    blockSrcs[reds] = {137,241,16,16};

    int greens = 5;
    srcVectors[greens].resize(2);
    srcVectors[greens][0] = {9,305,48,32};
    srcVectors[greens][1] = {81,297,32,48};
    dimensions[greens] = {3,2};
    bufferVectors[greens].resize(2);
    blockSrcs[greens] = {81,297,16,16};

    int square = 6;
    srcVectors[square].resize(1);
    srcVectors[square][0] = {145,305,32,32};
    dimensions[square] = {2,2};
    bufferVectors[square].resize(1);
    blockSrcs[square] = {145,305,16,16};

    //intialize each peice's buffer vector capacities
    for(int i = 0; i < bufferVectors.size(); i++)
        for(int h = 0; h < bufferVectors[i].size(); h++)
            bufferVectors[i][h].resize(dimensions[i].y);
    
    for(int i = 0; i < bufferVectors.size(); i++)
        for(int h = 0; h < bufferVectors[i].size(); h++)
            for(int j = 0; j < bufferVectors[i][h].size(); j++)
                bufferVectors[i][h][j].resize(dimensions[i].x);
    
    int buffer[18][10];
    for(int row = 0; row < 18; row++)
        for(int col = 0; col < 10; col++)
            buffer[row][col] = 0;

    //define buffer vectors
    bufferVectors[T][0] = {
        {0,1,0},
        {1,1,1}
    };
    bufferVectors[T][1]={
        {1,0},
        {1,1},
        {1,0}
    };
    bufferVectors[T][2]={
        {1,1,1},
        {0,1,0}
    };
    bufferVectors[T][3]={
        {0,1},
        {1,1},
        {0,1}
    };

    bufferVectors[orangel][0]={
        {0,0,2},
        {2,2,2}
    };
    bufferVectors[orangel][1]={
        {2,0},
        {2,0},
        {2,2}
    };
    bufferVectors[orangel][2]={
        {2,2,2},
        {2,0,0}
    };
    bufferVectors[orangel][3]={
        {2,2},
        {0,2},
        {0,2}
    };

    bufferVectors[bluel][0]={
        {3,0,0},
        {3,3,3}
    };
    bufferVectors[bluel][1]={
        {3,3},
        {3,0},
        {3,0}
    };
    bufferVectors[bluel][2]={
        {3,3,3},
        {0,0,3}
    };
    bufferVectors[bluel][3]={
        {0,3},
        {0,3},
        {3,3}
    };

    bufferVectors[pole][0]={
        {4,4,4,4}
    };
    bufferVectors[pole][1]={
        {4},
        {4},
        {4},
        {4}
    };

    bufferVectors[reds][0]={
        {5,5,0},
        {0,5,5}
    };
    bufferVectors[reds][1]={
        {0,5},
        {5,5},
        {5,0}
    };

    bufferVectors[greens][0]={
        {0,6,6},
        {6,6,0}
    };
    bufferVectors[greens][1]={
        {6,0},
        {6,6},
        {0,6}
    };

    bufferVectors[square][0] = {
        {7,7},
        {7,7}
    };

    SDL_Event e;
    int run = 1;
    std::vector<block> blocksCache;
    peice currentPeice;
    currentPeice.id = rand()%7;
    currentPeice.src = srcVectors[currentPeice.id][0];
    currentPeice.bufferPos = {5,0};
    currentPeice.dst = {currentPeice.bufferPos.x*celllength, currentPeice.bufferPos.y*celllength, celllength*dimensions[currentPeice.id].x, celllength*dimensions[currentPeice.id].y};
    currentPeice.dimensions = dimensions[currentPeice.id];
    currentPeice.rotationState = 0;
    
    Uint32 lastFall = SDL_GetTicks();
    while(run){
        std::vector<Vec2INT> eachRightmostYcords;
        for(int y = 0; y < currentPeice.dimensions.y; y++){
            for(int x = 0; x < currentPeice.dimensions.x; x++){
                if(x == currentPeice.dimensions.x-1)
                    if(bufferVectors[currentPeice.id][currentPeice.rotationState][y][x])
                        eachRightmostYcords.push_back({x,y});
                else if(bufferVectors[currentPeice.id][currentPeice.rotationState][y][x] && !bufferVectors[currentPeice.id][currentPeice.rotationState][y][x+1])
                    eachRightmostYcords.push_back({x,y});
            }
        }
        
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT)
                run = 0;

            if(e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_RIGHT){
                    for(int i = 0; i < eachRightmostYcords.size(); i++){
                        if(currentPeice.bufferPos.x+currentPeice.dimensions.x < 10 && !buffer[currentPeice.bufferPos.y+eachRightmostYcords[i].y][currentPeice.bufferPos.x+eachRightmostYcords[i].x+1]){
                            currentPeice.bufferPos.x += 1;
                            break;
                        }
                    }
                }
                if(e.key.keysym.sym == SDLK_LEFT){
                    if(currentPeice.bufferPos.x > 0 && buffer[currentPeice.bufferPos.y+currentPeice.dimensions.y-1][currentPeice.bufferPos.x-1] == 0)
                        currentPeice.bufferPos.x -= 1;
                }
                if(e.key.keysym.sym == SDLK_DOWN){
                    currentPeice.bufferPos.y += 1;
                }
                if(e.key.keysym.sym == SDLK_UP){
                    currentPeice.rotationState += 1;
                    if(currentPeice.rotationState > srcVectors[currentPeice.id].size()-1)
                        currentPeice.rotationState = 0;
                    //flip dimensions on rotate
                    int tempx = currentPeice.dimensions.x;
                    currentPeice.dimensions.x = currentPeice.dimensions.y;
                    currentPeice.dimensions.y = tempx;

                    //update peice src
                    currentPeice.src = srcVectors[currentPeice.id][currentPeice.rotationState];    
                }
            }
        }

        if(SDL_GetTicks() - lastFall >= 1000/level){
            currentPeice.bufferPos.y++;
            lastFall = SDL_GetTicks();
        }
        
        currentPeice.dst = {currentPeice.bufferPos.x*celllength, currentPeice.bufferPos.y*celllength, celllength*currentPeice.dimensions.x, celllength*currentPeice.dimensions.y};

        std::vector<Vec2INT> eachLeftmostYcords;
        
        //IMPLEMENT HORIZONTAL COLLISION CHECK FOR EACH ROW
        std::vector<Vec2INT> eachCurrentPeicebottomXCords;
        for(int x = 0; x < currentPeice.dimensions.x; x++){
            for(int y = 0; y < currentPeice.dimensions.y; y++){
                if(y == currentPeice.dimensions.y-1){
                    if(bufferVectors[currentPeice.id][currentPeice.rotationState][y][x])
                        eachCurrentPeicebottomXCords.push_back({x,y});
                }else if(bufferVectors[currentPeice.id][currentPeice.rotationState][y][x] && !bufferVectors[currentPeice.id][currentPeice.rotationState][y+1][x])
                    eachCurrentPeicebottomXCords.push_back({x,y});
            }
        }
        
        for(int i = 0; i < eachCurrentPeicebottomXCords.size(); i++){
            if(currentPeice.bufferPos.y == 18 - currentPeice.dimensions.y || buffer[currentPeice.bufferPos.y + eachCurrentPeicebottomXCords[i].y+1][currentPeice.bufferPos.x+eachCurrentPeicebottomXCords[i].x]){
                //convert peice to blocks
                for(int y = 0; y < currentPeice.dimensions.y; y++){
                    for(int x = 0; x < currentPeice.dimensions.x; x++){
                        if(bufferVectors[currentPeice.id][currentPeice.rotationState][y][x]){
                            block newBlock;
                            newBlock.id = currentPeice.id;
                            newBlock.src = blockSrcs[newBlock.id];
                            blocksCache.push_back(newBlock);
                            buffer[currentPeice.bufferPos.y+y][currentPeice.bufferPos.x+x] = newBlock.id+1;
                        }    
                    }
                }
                
                //change peice
                currentPeice.id = rand()%7;
                currentPeice.src = srcVectors[currentPeice.id][0];
                currentPeice.bufferPos = {5,0};
                currentPeice.dst = {currentPeice.bufferPos.x*celllength, currentPeice.bufferPos.y*celllength, celllength*dimensions[currentPeice.id].x, celllength*dimensions[currentPeice.id].y};
                currentPeice.dimensions = dimensions[currentPeice.id];
                currentPeice.rotationState = 0;
                
                //row clear
                for(int row = 0; row < 18; row++){
                    int colCount = 0;
                    //check if row is full
                    for(int col = 0; col < 10; col ++){
                        if(buffer[row][col]){
                            colCount++;
                        }
                    }
                    //clear and shift down logic buffer and srcs
                    if(colCount == 10){
                        score += 100*level;
                        
                        std::vector<std::vector<int>> bufferTemp;
                        bufferTemp.resize(row);
                        for(int temprow = 0; temprow < row; temprow++)
                            bufferTemp[temprow].resize(10);
                        for(int y = 0; y < row; y++)
                            for(int x = 0; x < 10; x++)
                                bufferTemp[y][x] = buffer[y][x];
                        
                        for(int clearrow = 0; clearrow < row; clearrow++){
                            for(int clearcol = 0; clearcol < 10; clearcol++)
                                buffer[clearrow][clearcol] = 0;
                        }
                        
                        //shift down logic buffer
                        for(int y = 0; y < row; y++)
                            for(int x = 0; x < 10; x++)
                                buffer[y+1][x] = bufferTemp[y][x];
                        row--;
                    }else
                        colCount = 0;
                }
                break;    
            }
        }

        for(int x = 0; x < 10; x++)
            if(buffer[0][x])
                run = 0;

        SDL_RenderClear(renderer.get());
        //render buffer
        for(int y = 0; y < 18; y++){
            for(int x = 0; x < 10; x++){
                if(buffer[y][x]){
                    SDL_Rect dst = {x*celllength,y*celllength,celllength,celllength};
                    SDL_RenderCopy(renderer.get(), spriteSheetTexture.get(), &blockSrcs[buffer[y][x]-1], &dst);
                }
            }
        }
        SDL_RenderCopy(renderer.get(), spriteSheetTexture.get(), &currentPeice.src, &currentPeice.dst);
        SDL_RenderPresent(renderer.get());
    }

    std::cout << "Score: " << score << "\n";
}