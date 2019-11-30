#pragma once

#include <iostream>

enum NodeState
{
    START,
    END,
    SETTLED,
    UNSETTLED,
    SELECTED,
    OBSTACLE,
    PATH,
    NONE
};

struct Node
{
    Node* parentNode;
	double fCost;
	int gCost;
	double hCost;
	int weight;

	int PosX;
	int PosY;
	NodeState nodeState;
	bool expand = false;

	//Normal Constructor
	Node()
	{
        gCost = 0;
        hCost = 0;
        weight = 0;
        fCost = gCost + weight * hCost;

        PosX = -1;
        PosY = -1;
        nodeState = NodeState::UNSETTLED;
        parentNode = NULL;
	}

	Node(int x, int y, NodeState nState)
	{
        gCost = 0;
        hCost = 0;
        weight = 0;
        fCost = gCost + weight * hCost;

        PosX = x;
        PosY = y;
        nodeState = nState;
        parentNode = NULL;
	}

    //Reset
	void ResetNode()
	{
        gCost = 0;
        hCost = 0;
        weight = 0;
        fCost = 0;
        nodeState = NodeState::UNSETTLED;
        parentNode = NULL;
	}

	void SetCost(int g_Cost, int h_Cost, int w)
	{
	    gCost = g_Cost;
        hCost = h_Cost;
        weight = w;
        fCost = gCost + weight * hCost;
	}

	void SetGCost(int g_Cost)
	{
	    gCost = g_Cost;
        fCost = gCost + weight * hCost;
	}

	void SetCostState(int g_Cost, double h_Cost, int w, NodeState nState)
	{
	    gCost = g_Cost;
        hCost = h_Cost;
        weight = w;
        fCost = gCost + weight * hCost;
        nodeState = nState;
	}
};

struct NodeOrder
{
    bool operator()(const Node* a, const Node* b)
    {
        return a->fCost>b->fCost;
    }
};
