#include <GL/freeglut.h>
#include <vector>
#include <stdlib.h>

void board_maker()
{
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            if (makeboard[i][j] == 1)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    draw_block();
                }
                glPopMatrix(); // 블럭 표시

                glColor3f(1, 0, 0);
                glLineWidth(1);
                glBegin(GL_LINE_LOOP);
                {
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                }
                glEnd();
                glLineWidth(1); // 블럭 위치
            }
            else if (makeboard[i][j] == 2)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 0, j * 1.1 - 15);
                    item();
                }
                glPopMatrix(); // 아이템 표시

                glColor3f(0, 1, 0);
                glLineWidth(1);
                glBegin(GL_LINE_LOOP);
                {
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                }
                glEnd();
                glLineWidth(1); // 아이템 위치
            }
            else if (makeboard[i][j] == 3)
            {
                glPushMatrix();
                {
                    glTranslatef(i * 1.1 - 15, 1.5, j * 1.1 - 15);
                    glColor3f(0.0, 0.0, 0.0);

                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                    //오른쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                    }
                    glEnd();
                    //뒷쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                    }
                    glEnd();
                    ////왼쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                    ////아랫쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, -2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, -2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, -2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, -2.0f, 1.0f);
                    }
                    glEnd();
                    ////윗쪽
                    glBindTexture(GL_TEXTURE_2D, textures[1]);
                    glBegin(GL_QUADS);
                    {
                        glTexCoord2f(1, 1);
                        glVertex3f(-1.0f, 2.0f, 1.0f);
                        glTexCoord2f(0, 1);
                        glVertex3f(-1.0f, 2.0f, -1.0f);
                        glTexCoord2f(0, 0);
                        glVertex3f(1.0f, 2.0f, -1.0f);
                        glTexCoord2f(1, 0);
                        glVertex3f(1.0f, 2.0f, 1.0f);
                    }
                    glEnd();
                }
                glPopMatrix(); // 도착지 표시

                glColor3f(0, 0, 1);
                glLineWidth(1);
                glBegin(GL_LINE_LOOP);
                {
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].max_z);
                    glVertex3f(static_block[i][j].min_x, 1, static_block[i][j].min_z);
                    glVertex3f(static_block[i][j].max_x, 1, static_block[i][j].min_z);
                }
                glEnd();
                glLineWidth(1); // 도착지 위치
            }
        }
    }
}
