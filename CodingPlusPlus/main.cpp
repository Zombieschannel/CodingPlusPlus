#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;

struct Raycaster
{
    //const
    const int rayAmount = 1280;
    const int fov = 70;
    const float closest = 0;
    const float renderDistance = 450;
    const int hitDistance = 30;
    //important
    RenderWindow window;
    vector<vector<int>> blockMap;
    Image img;
    Texture minimap;
    RectangleShape miniMap;
    CircleShape player;
    Vector2f playerPos;
    float scale;
    float playerSpeed = 0.03;
    float rotateSpeed = 1;
    float playerDirection = 0;
    vector<Vector2f> rayPositions;
    vector<bool> whereHits;
    Vertex VisualRay[2];
    Vertex ScreenLine[2];
    Texture blockTexture;
    Texture blockTexture2;
    RectangleShape floor;
    RectangleShape sky;
    float distance = 0;
    //game
    void castRays() //outdated
    {

        for (int i = 0; i < rayAmount; i++)
        {
            rayPositions[i] = playerPos; //reset ray position
            float angle = playerDirection - fov / 2 + ((float)fov / rayAmount) * i; //calculate angle of a ray
            Vector2f direction = Vector2f(cos(angle * 0.0174533) / 100, sin(angle * 0.0174533) / 100); //calculate direction of a ray
            for (int j = 0; j < 100000; j++)
            {
                rayPositions[i] += direction; //move the ray
                if (blockMap[rayPositions[i].x][rayPositions[i].y] == 1) //check if ray hit a block
                {
                    break;
                }
            }
        }

    }
    void BetterCastRays()
    {
        for (int i = 0; i < rayAmount; i++)
        {
            bool whereHit = 0; // 0 if hit on the X axis, 1 if hit on the Y axis
            rayPositions[i] = playerPos; //reset ray position
            float angle = playerDirection - fov / 2 + ((float)fov / rayAmount) * i; //calculate angle of a ray
            Vector2f direction = Vector2f(cos(angle * 0.0174533), sin(angle * 0.0174533)); //calculate direction of a ray
            float maximum = max(abs(direction.x), abs(direction.y)); //find the max to be able to make one of the axis = 1
            direction.x /= maximum;
            direction.y /= maximum;
            for (int j = 0; j < hitDistance; j++)
            {
                Vector2f toTheEdge; //how far to an edge of a block
                if (direction.x > 0)
                    toTheEdge.x = 1 - (rayPositions[i].x - (int)rayPositions[i].x);//how far to an edge if looking to the right
                else
                    toTheEdge.x = -(rayPositions[i].x - (int)rayPositions[i].x);//how far to an edge if looking to the left
                if (direction.y > 0)
                    toTheEdge.y = 1 - (rayPositions[i].y - (int)rayPositions[i].y);//how far to an edge if looking to the down
                else
                    toTheEdge.y = -(rayPositions[i].y - (int)rayPositions[i].y);//how far to an edge if looking to the up
                Vector2f toEdgeVectorX;
                Vector2f toEdgeVectorY;
                toEdgeVectorX = Vector2f(toTheEdge.x, abs(toTheEdge.x / direction.x) * direction.y);
                toEdgeVectorY = Vector2f(abs(toTheEdge.y / direction.y) * direction.x, toTheEdge.y);
                if (toEdgeVectorX.x * toEdgeVectorX.x + toEdgeVectorX.y * toEdgeVectorX.y < //compare which edge we hit first
                    toEdgeVectorY.x * toEdgeVectorY.x + toEdgeVectorY.y * toEdgeVectorY.y)
                {
                    rayPositions[i] += toEdgeVectorX; //move the ray to the X edge
                    if (toEdgeVectorY.x < 0)
                        rayPositions[i].x -= 0.00001;//subtract a small number so that it isn't on the edge
                    whereHit = 0;
                }
                else
                {
                    rayPositions[i] += toEdgeVectorY; //move the ray to the Y edge
                    if (toEdgeVectorY.y < 0)
                        rayPositions[i].y -= 0.00001;//subtract a small number so that it isn't on the edge
                    whereHit = 1;
                }
                if (blockMap[(int)rayPositions[i].x][(int)rayPositions[i].y] != 0) //check if ray hit a block
                    break;
            }
            whereHits[i] = whereHit;

        }
    }
    void loadImg()
    {
        blockMap.resize(img.getSize().x);
        for (int i = 0; i < img.getSize().y; i++)
            blockMap[i].resize(img.getSize().y);
        for (int i = 0; i < img.getSize().x; i++)
            for (int j = 0; j < img.getSize().y; j++)
                blockMap[i][j] = (img.getPixel(i, j).r > 0 ? 1 : 0) + (img.getPixel(i, j).g > 0 ? 2 : 0);
    }
    void Start()
    {
        window.create(VideoMode(1280, 720), "", Style::Default);
        img.loadFromFile("Res/img1.png");
        blockTexture.loadFromFile("Res/Texture1.png");
        blockTexture2.loadFromFile("Res/Texture2.png");
        minimap.loadFromImage(img);
        miniMap.setTexture(&minimap);
        //miniMap.setSize(Vector2f(160, 160));
        miniMap.setSize(Vector2f(240, 240));
        scale = (float)miniMap.getSize().x / img.getSize().x;
        rayPositions.resize(rayAmount);
        whereHits.resize(rayAmount);
        miniMap.setOutlineColor(Color::White);
        miniMap.setOutlineThickness(1);
        VisualRay[0].color = Color::Blue;
        VisualRay[1].color = Color::Blue;
        ScreenLine[0].color = Color::White;
        ScreenLine[1].color = Color::White;
        floor.setSize(Vector2f(window.getSize().x, window.getSize().y / 2));
        sky.setSize(Vector2f(window.getSize().x, window.getSize().y / 2));
        floor.setPosition(0, window.getSize().y / 2);
        floor.setFillColor(Color(128, 128, 128));
        sky.setFillColor(Color(96, 96, 96));
        loadImg();
        player.setRadius(scale / 4);
        player.setOrigin(scale / 4, scale / 4);
        playerPos = Vector2f(10.5, 10.7);
        app();
    }
    void app()
    {
        Clock clocK;
        while (window.isOpen())
        {
            clocK.restart();
            Event event;
            while (window.pollEvent(event))
            {
                switch (event.type)
                {
                case Event::Closed:
                    window.close();
                    break;
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::A))
                playerDirection += -rotateSpeed;
            if (Keyboard::isKeyPressed(Keyboard::D))
                playerDirection += rotateSpeed;
            if (Keyboard::isKeyPressed(Keyboard::W))
                playerPos += Vector2f(cos(playerDirection * 0.0174533) * playerSpeed, sin(playerDirection * 0.0174533) * playerSpeed);
            if (Keyboard::isKeyPressed(Keyboard::S))
                playerPos += Vector2f(cos(playerDirection * 0.0174533) * -playerSpeed, sin(playerDirection * 0.0174533) * -playerSpeed);
            if (Keyboard::isKeyPressed(Keyboard::F))
                distance = 0;
            if (Keyboard::isKeyPressed(Keyboard::G))
                distance += 0.01;
            player.setPosition(scale * playerPos.x, scale * playerPos.y);

            BetterCastRays();
            window.clear();
            window.draw(floor);
            window.draw(sky);
            for (int i = 0; i < window.getSize().x; i++)
            {
                int rayID = ((float)rayAmount / window.getSize().x) * i;
                Vector2f rayDir = rayPositions[rayID] - playerPos;
                float RayDistance = sqrt(rayDir.x * rayDir.x + rayDir.y * rayDir.y)
                    * cos(-fov / 2 + ((float)fov / rayAmount) * rayID * 0.0174533); // to avoid fisheye effect
                if (RayDistance == 0)
                    RayDistance = 0.000001;
                ScreenLine[0].position = Vector2f(i + distance * (rayID - (signed int)window.getSize().x / 2), window.getSize().y / 2 - renderDistance / RayDistance);
                ScreenLine[1].position = Vector2f(i + distance * (rayID - (signed int)window.getSize().x / 2), window.getSize().y / 2 + renderDistance / RayDistance);
                Vector2f rayNormalized = Vector2f(rayPositions[rayID].x - (int)rayPositions[rayID].x, rayPositions[rayID].y - (int)rayPositions[rayID].y);
                ScreenLine[0].texCoords = Vector2f(blockTexture.getSize().x * (whereHits[rayID] == 0 ? rayNormalized.y : rayNormalized.x), 0);
                ScreenLine[1].texCoords = Vector2f(blockTexture.getSize().x * (whereHits[rayID] == 0 ? rayNormalized.y : rayNormalized.x), blockTexture.getSize().y);
                switch (blockMap[(int)rayPositions[i].x][(int)rayPositions[i].y])
                {
                case 1:
                    window.draw(ScreenLine, 2, Lines, &blockTexture);
                    break;
                case 2:
                    window.draw(ScreenLine, 2, Lines, &blockTexture2);
                    break;
                }

            }
            window.draw(miniMap);
            for (int i = 0; i < rayAmount; i++)
            {
                VisualRay[0].position = Vector2f(scale * playerPos.x, scale * playerPos.y);
                VisualRay[1].position = Vector2f(scale * rayPositions[i].x, scale * rayPositions[i].y);
                window.draw(VisualRay, 2, Lines);
            }

            window.draw(player);
            window.display();
            window.setTitle(to_string(int(1 / clocK.getElapsedTime().asSeconds())));
            if (clocK.getElapsedTime().asMicroseconds() < 16666)
                sleep(microseconds(16666 - clocK.getElapsedTime().asMicroseconds()));
        }
    }
};

int main()
{
    Raycaster app;
    app.Start();
}
