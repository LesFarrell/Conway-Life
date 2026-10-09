#include <stdio.h>
#include "tigr.h"

#define ScreenWidth     320
#define ScreenHeight    200

void ProcessGeneration(Tigr *Source, Tigr *Destination);

//-----------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int MX, MY, Button;
    int mouseclicked = 0;

    Tigr *Source = tigrBitmap(ScreenWidth, ScreenHeight);
    Tigr *Destination = tigrBitmap(ScreenWidth, ScreenHeight);
    Tigr *Screen = tigrWindow(ScreenWidth, ScreenHeight, "Conway's Game of Life", TIGR_4X);

    tigrClear(Source, tigrRGB(0x0, 0x0, 0x0));

    while (!tigrClosed(Screen) && !tigrKeyDown(Screen, TK_ESCAPE))
    {
        // Clear the Screen
        tigrClear(Screen, tigrRGB(0x0, 0x0, 0x0));

        // Process a Generation
        if (tigrKeyHeld(Screen, TK_SPACE))
        {
            ProcessGeneration(Source, Destination);
            mouseclicked = 1;
        }

        // Clear the grid
        if (tigrKeyHeld(Screen, 'C'))
        {
            tigrClear(Source, tigrRGB(0x0, 0x0, 0x0));
        }

        // Plot a point on screen at the mouse position.
        tigrMouse(Screen, &MX, &MY, &Button);
        if (Button != 0) {
            mouseclicked = 1;
            tigrPlot(Source, MX, MY, tigrRGB(0xff, 0xff, 0xff));
        }

        // Copy the Source to Screen.
        tigrBlit(Screen, Source, 0, 0, 0, 0, ScreenWidth, ScreenHeight);

        if (mouseclicked == 0)
            tigrPrint(Screen, tfont, 6, 100, tigrRGB(0xff, 0xff, 0xff), "Use mouse to draw, 'Space' for next generation.");

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

// A cell is alive if it isn't black.
static int IsAlive(Tigr *Bitmap, int x, int y)
{
    TPixel colour = tigrGet(Bitmap, x, y);
    return colour.r != 0x0 || colour.g != 0x0 || colour.b != 0x0;
}

//-----------------------------------------------------------------------------------------------------------

void ProcessGeneration(Tigr *Source, Tigr *Destination)
{
    // Loop for each cell on screen.
    for (int y = 0; y < ScreenHeight; y++)
    {
        for (int x = 0; x < ScreenWidth; x++)
        {
            // Count the live neighbours, wrapping around the edges of the grid.
            int Neighbours = 0;
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dx == 0 && dy == 0) continue;

                    int nx = (x + dx + ScreenWidth) % ScreenWidth;
                    int ny = (y + dy + ScreenHeight) % ScreenHeight;

                    if (IsAlive(Source, nx, ny)) Neighbours++;
                }
            }

            // Survival: a live cell with two or three neighbours lives on.
            // Birth: a dead cell with exactly three neighbours becomes live.
            // Anything else dies (isolation or overcrowding) or stays dead.
            int Alive = IsAlive(Source, x, y);
            int NextAlive = Neighbours == 3 || (Alive && Neighbours == 2);

            // Always write the cell so no stale state is left in Destination.
            tigrPlot(Destination, x, y, NextAlive ? tigrRGB(0xff, 0xff, 0xff) : tigrRGB(0x0, 0x0, 0x0));
        }
    }

    // Copy the updated destination bitmap to the source bitmap.
    tigrBlit(Source, Destination, 0, 0, 0, 0, ScreenWidth, ScreenHeight);
}

//-----------------------------------------------------------------------------------------------------------
