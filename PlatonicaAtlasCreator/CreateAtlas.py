from PIL import Image

blocks = [
"dirt",
"grass",
"grassTop",
"stone",
"log",
"ruggedStone",
"leaves"]

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


if __name__ == "__main__":
    main()