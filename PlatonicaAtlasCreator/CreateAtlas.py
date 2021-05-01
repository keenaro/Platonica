from PIL import Image
import subprocess

blocks = [
"dirt",
"grass",
"grassTop",
"stone",
"log",
"ruggedStone",
"leaves",
"ice",
"water",
"bricks_red",
"sand",
"cactus_bottom",
"cactus_side",
"bricks_black",
"planks_birch",
"planks_pine",
"planks_oak",
"chest_bottom",
"chest_side",
"chest_front",
"chest_back",
"furnace_bottom",
"furnace_side",
"furnace_front" ]

isLocatedWithinPlatonicaDirectory = True

def CopyTexture(atlas, textureName, position):
    texture = Image.open("blocks/" + textureName + ".png")
    texture.copy()
    atlas.paste(texture, position)

def main():
    atlas = Image.new('RGBA', (1024, 1024), (0,0,0,0))
    textureWidth = 16
   
    positionX = 0
    positionY = 0
    
    for block in blocks:
        CopyTexture(atlas, block, (positionX, positionY))
        positionX+=textureWidth
        if(positionX == 1024):
            positionY += textureWidth
            positionX = 0
    
    atlas.save('atlas.png')
    
    if isLocatedWithinPlatonicaDirectory:
        atlas.save('..\\resources\\textures\\atlas.png')

if __name__ == "__main__":
    main()