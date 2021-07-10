#include <stdio.h>
#include "tigr.h"

#define ScreenWidth     320
#define ScreenHeight    200

#define DEAD            0
#define ALIVE           1

int ProcessGeneration(Tigr *Source, Tigr *Destination);

//-----------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int MX, MY, Button;
    int mouseclicked=0;

    Tigr *Source = tigrBitmap(ScreenWidth, ScreenHeight);
    Tigr *Destination = tigrBitmap(ScreenWidth, ScreenHeight);
    Tigr *Screen = tigrWindow(320, 200, "Conway's Game of Life", TIGR_4X);

    tigrClear(Source, tigrRGB(0x0, 0x0, 0x0));

    while (!tigrClosed(Screen) && !tigrKeyDown(Screen,TK_ESCAPE))
    {
        // Clear the Screen
        tigrClear(Screen, tigrRGB(0x0, 0x0, 0x0));

        // Process a Generation
        if (tigrKeyHeld(Screen,TK_SPACE))
        { 
            ProcessGeneration(Source, Destination);
            mouseclicked = 1;
        }

        // Clear the screen
        if (tigrKeyHeld(Screen,'C'))
        { 
            tigrClear(Source, tigrRGB(0x0, 0x0, 0x0));
        }

        // Plot a point on screen at the mouse position.
        tigrMouse(Screen,&MX, &MY, &Button);
        if (Button !=0) {   
            mouseclicked = 1;
            tigrPlot(Source, MX, MY, tigrRGB(0xff, 0xff, 0xff));
        }

        // Copy the Source to Screen.
        tigrBlit(Screen, Source, 0, 0, 0, 0, ScreenWidth, ScreenHeight);

        if (mouseclicked == 0) 
            tigrPrint(Screen, tfont, 6, 100,tigrRGB(0xff, 0xff, 0xff), "Use mouse to draw, 'Space' for next generation.");

        // Update the Screen
        tigrUpdate(Screen);
    }

    // Free the Tigr Bitmaps
    tigrFree(Screen);
    tigrFree(Source);
    tigrFree(Destination);

    return 0;
}

//-----------------------------------------------------------------------------------------------------------

int ProcessGeneration(Tigr *Source, Tigr *Destination)
{
    int nx = 0;
    int ny = 0;
    int CellState = DEAD;
    int Neighbours = 0;
    TPixel colour;

    // Loop for each cell on screen.
    for (int y = 0; y < ScreenHeight; y++)
    {
        for (int x = 0; x < ScreenWidth; x++)
        {
            // Is the current cell Dead or Alive?
            colour = tigrGet(Source, x, y);
            if ( colour.r == 0x0 && colour.g == 0x0 && colour.b == 0x0 ) CellState = DEAD; else CellState = ALIVE;

            // Count the number of neighbours the cell has.
            nx = x;
            ny = y;
            Neighbours = 0;
            for (int Pos = 0; Pos < 8; Pos++)
            {
                // Check rows above the current position.
                if (Pos == 0) { nx = x - 1; ny = y - 1; }
                if (Pos == 1) { nx = x    ; ny = y - 1; }
                if (Pos == 2) { nx = x + 1; ny = y - 1; }

                // Check the cells to the left and right.
                if (Pos == 3) { nx = x - 1; ny = y; }
                if (Pos == 4) { nx = x + 1; ny = y; }

                // Check rows below the current position.
                if (Pos == 5) { nx = x - 1; ny = y + 1; }
                if (Pos == 6) { nx = x    ; ny = y + 1; }
                if (Pos == 7) { nx = x + 1; ny = y + 1; }

                // Wrap the coordinates if they fall off screen.
                if (nx < 0 ) nx = ScreenWidth - 1;
                if (ny < 0 ) ny = ScreenHeight - 1;
                if (nx > ScreenWidth - 1 )  nx = 0;
                if (ny > ScreenHeight - 1 ) ny = 0;

                // Check the colour at the current point.
                colour = tigrGet(Source, nx, ny);

                // If it's not black then it must be a Neighbour, so increase the Neighbour count.
                if ( colour.r != 0x0 && colour.g != 0x0 && colour.b != 0x0 ) Neighbours++;
            }

            // Death by isolation: Each live cell with one or fewer live neighbours will die!
            if (CellState == ALIVE && Neighbours <= 1)  tigrPlot(Destination, x, y, tigrRGB(0, 0x0, 0x0));

            // Survival: Each live cell with either two or three live neighbours will remain alive.
            if (CellState == ALIVE && (Neighbours == 2 || Neighbours == 3)) tigrPlot(Destination, x, y,tigrRGB(0xff, 0xff, 0xff));

            // Births: Each dead cell adjacent to exactly three live neighbours will become live!.
            if (CellState == DEAD && Neighbours ==  3) tigrPlot(Destination, x, y, tigrRGB(0xff, 0xff, 0xff));

            // Death by overcrowding: Each live cell with four or more live neighbours will die!.
            if (CellState == ALIVE && Neighbours >= 4) tigrPlot(Destination, x, y, tigrRGB(0x0, 0x0, 0x0));
        }
    }

    // Copy the updated destination bitmap to the source bitmap.
    tigrBlit(Source, Destination, 0, 0, 0, 0, ScreenWidth, ScreenHeight);
}


//-----------------------------------------------------------------------------------------------------------
