#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <math.h>

#include "Node.h"

const int rows = 40; // x
const int cols = 40; // y

Node Grid[cols][rows];

Node* startingNode = NULL;
Node* endingNode = NULL;
std::priority_queue< Node*, std::vector<Node*>, NodeOrder >* openNodeList = new std::priority_queue< Node*, std::vector<Node*>, NodeOrder >();

enum HeuristicsMode
{
    Euclidean,
    Manhattan,
    Octile,
    Chebyshev
};

void NodeColor(NodeState nState)
{
    if(nState == NodeState::UNSETTLED)
    {
        glColor3f(1.0f, 1.0f, 1.0f); //! White
    }
    else if(nState == NodeState::OBSTACLE)
    {
        glColor3f(0.32f, 0.32f, 0.32f); //! Grey
    }
    else if(nState == NodeState::START)
    {
        glColor3f(0.0f, 0.96f, 0.92f); //! Cyan
    }
    else if(nState == NodeState::END)
    {
        glColor3f(0.96f, 0.0f, 0.76f); //! Purple
    }
    else if(nState == NodeState::SELECTED) // Selected comes before settled
    {
        glColor3f(0.3f, 1.0f, 0.14f); //! Light Green
    }
    else if(nState == NodeState::SETTLED)
    {
        glColor3f(0.0f, 0.32f, 0.03f); //! Green
    }
    else if(nState == NodeState::PATH)
    {
        glColor3f(1.0f, 1.0f, 0.0f); //! Yellow
    }
}

void SetUpPathGrid()
{
    for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			Grid[i][j] = Node(i,j,NodeState::UNSETTLED);
		}
	}
	startingNode = &Grid[3][3];
	endingNode = &Grid[20][7];
	startingNode->nodeState = NodeState::START;
	endingNode->nodeState = NodeState::END;
	openNodeList->push(startingNode);
}

void SelectPathGrid(int x, int y, int button)
{
    static NodeState* prevState = NULL;
    Node* temp = &Grid[x][y];
    NodeState* nState = &Grid[x][y].nodeState;

    if(button == GLFW_PRESS)
    {
        if(prevState != NULL)
        {
            if(*prevState == NodeState::START || *prevState == NodeState::END)
            {
                if(*nState == NodeState::UNSETTLED)
                {
                    *nState = *prevState;
                    *prevState = NodeState::UNSETTLED;
                    prevState = nState;
                    if(*nState == NodeState::START)
                    {
                        openNodeList->pop();
                        startingNode = temp;
                        openNodeList->push(startingNode);
                    }
                    if(*nState == NodeState::END)
                    {
                        endingNode = temp;
                    }
                    temp->expand = true;
                }
            }
            else if(*prevState == NodeState::UNSETTLED || *prevState == NodeState::OBSTACLE)
            {
                if(*nState == NodeState::UNSETTLED || *nState == NodeState::OBSTACLE)
                {
                    *nState = *prevState;
                    temp->expand = true;
                }
            }
        }
        else if(prevState == NULL)
        {
            if(*nState == NodeState::START || *nState == NodeState::END)
            {
                prevState = nState;
            }
            else if(*nState == NodeState::UNSETTLED)
            {
                prevState = nState;
                *prevState = NodeState::OBSTACLE;
            }
            else if(*nState == NodeState::OBSTACLE)
            {
                prevState = nState;
                *prevState = NodeState::UNSETTLED;
            }
        }
    }
    if(button == GLFW_RELEASE)
    {
        prevState = NULL;
    }
}

double Heuristics(Node coord1, Node coord2, HeuristicsMode mode)
{
    int x1 = coord1.PosX, y1 = coord1.PosY, x2 = coord2.PosX, y2 = coord2.PosY;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if(mode == HeuristicsMode::Euclidean)
    {
        return sqrt(dx * dx + dy * dy) * 10;
    }
    else if(mode == HeuristicsMode::Manhattan)
    {
        return (dx + dy) * 10;
    }
    else if(mode == HeuristicsMode::Octile)
    {
        double F = (sqrt(2) - 1);
        return (dx < dy) ? (F * dx + dy) * 10 : (F * dy + dx) * 10;
    }
    else if(mode == HeuristicsMode::Chebyshev)
    {
        return (max(dx, dy) * 10);
    }
}

void PathRender(Node* endNode)
{
    endNode->nodeState = NodeState::PATH;
    if(endNode->parentNode->parentNode != NULL)
    {
        PathRender(endNode->parentNode);
    }
}

void ResetGrid()
{
    while(openNodeList->empty() == false)
    {
        openNodeList->pop();
    }

    for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			if(Grid[i][j].nodeState != NodeState::OBSTACLE)
            {
                Grid[i][j].ResetNode();
            }
		}
	}

	startingNode->nodeState = NodeState::START;
	endingNode->nodeState = NodeState::END;

	openNodeList->push(startingNode);
}

bool CalculatePathGrid(bool doDiagonal, int weight, HeuristicsMode mode)
{
    Node* currentNode = openNodeList->top();
    openNodeList->pop();

    if(currentNode != NULL)
    {
        int curX = currentNode->PosX;
        int curY = currentNode->PosY;

        int curGCost = currentNode->gCost;

        if(currentNode->nodeState == NodeState::START)
        {
            currentNode->SetCost(0,0,0);
        }

        if(currentNode->nodeState == NodeState::SELECTED)
        {
            currentNode->nodeState = NodeState::SETTLED;
        }

        for(int i = -1; i < 2; i++)
        {
            for(int j = -1; j < 2; j++)
            {
                if(i == 0 && j == 0) { }
                else
                {
                    if(curX + i >= 0 && curY + j >= 0 && curX + i < cols && curY + j < rows)
                    {
                        Node temp = Grid[curX + i][curY + j];
                        Node* pNode = &Grid[curX + i][curY + j];
                        int gCost;
                        double hCost = Heuristics(temp, *endingNode, mode);

                        if(i != 0 && j != 0)
                        {
                            if(doDiagonal == false) { continue; }
                            gCost = 14;
                        }
                        else { gCost = 10; }
                        gCost += curGCost;

                        if(temp.nodeState == NodeState::UNSETTLED ||
                           temp.nodeState == NodeState::SELECTED ||
                           temp.nodeState == NodeState::END)
                        {
                            if(temp.nodeState == NodeState::SELECTED || temp.nodeState == NodeState::END)
                            {
                                if(temp.gCost > gCost)
                                {
                                    pNode->SetGCost(gCost);
                                    pNode->parentNode = currentNode;
                                }
                            }

                            if (temp.fCost == 0)
                            {
                                pNode->SetCost(gCost, hCost, weight);
                                pNode->parentNode = currentNode;

                                if(temp.nodeState == NodeState::UNSETTLED)
                                {
                                    pNode->nodeState = NodeState::SELECTED;
                                }
                                openNodeList->push(pNode);
                            }

                            if(temp.nodeState == NodeState::END)
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

void DrawPathGrid(int width, int height)
{
	int xMin = width - height;//ratioW * 5;
	int xMax = width;

	int yMin = 0;
	int yMax = height;

	int xOffset = (xMax - xMin) / cols;
	int yOffset = (yMax - yMin) / rows;


	float offset = 1.0;
	float offset1 = 1.0;
	float offset2 = -1.2;

	glBegin(GL_QUADS);

	for(int i = 0; i<cols; i++)
    {
        float xPos =  xMin + i * xOffset;

        for(int j = 0; j<rows; j++)
        {
            float yPos =  yMin + j * yOffset;

            if(Grid[i][j].expand == true)
            {
                offset = offset2;
                Grid[i][j].expand = false;
            }
            else
            {
                offset = offset1;
            }

            NodeColor(Grid[i][j].nodeState);

            glVertex2f(xPos + offset, yPos + offset);
			glVertex2f(xPos + xOffset - offset, yPos + offset);
			glVertex2f(xPos + xOffset - offset, yPos + yOffset - offset);
			glVertex2f(xPos + offset, yPos + yOffset - offset);

        }
    }

	glEnd();
}
