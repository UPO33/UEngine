#include <Windows.h>
#include <Windowsx.h>

#include "../Core/Array.h"
#include "../Core/Int2.h"
#include "Algo/"
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>


using namespace UCore;

HWND gWnd;

struct PFNode
{
	struct Linkage
	{
		PFNode*	mNode;
		float	mDistance;
	};

	TArray<Linkage> mLinkages;
	int mLayer;
};

struct PFNodeMapping
{
	int mFScore;
	int mGScore;
	unsigned mIsInOpenList : 1;
	unsigned mIsInClosedList : 1;
	PFNode* mCameFrom;
};

struct PFGraph
{
	TArray<PFNode*>	mNodes;
};

struct AStarCell
{
	size_t mIsBlock : 1;
	size_t mIsInClosedList : 1;
	size_t mIsInOpenList : 1;
	int mGScore;
	int mFScore;
	AStarCell* mCameFrom;
};

struct AStarGrid
{
	unsigned	mNumCellInX = 64;
	unsigned	mNumCellInY = 64;

	TArray<AStarCell>	mCells;
	Int2				mSrcCellIndex = Int2(-1);
	Int2				mDstCellIndex = Int2(-1);
	TArray<Int2>		mOpenList;
	TArray<AStarCell*>	mResultPathPtr;

	AStarGrid()
	{
		FillRnd();
	}
	AStarCell* GetCell(Int2 index)
	{
		return GetCell(index.x, index.y);
	}
	AStarCell* GetCell(unsigned x, unsigned y)
	{
		return &(mCells[y * mNumCellInX + x]);
	}
	void FillRnd()
	{
		srand(GetTickCount());

		mCells.RemoveAll();
		mCells.AddUnInit(mNumCellInX * mNumCellInY);

		{
			for (AStarCell& cell : mCells)
			{
				cell.mIsBlock = false; // rand() % 3 == 0;
			}
		}
		unsigned nGroup = mNumCellInX / 2;

		for (unsigned i = 0; i < nGroup; i++)
		{
			unsigned sx = rand() % mNumCellInX;
			unsigned sy = rand() % mNumCellInY;

			unsigned count = 1 + (rand() % 6);
			bool bVertical = rand() % 2 == 0;

			for (unsigned iCount = 0; iCount < count; iCount++)
			{
				if (bVertical)
				{
					mCells[((sy + iCount) % mNumCellInY * mNumCellInX) + sx].mIsBlock = true;
				}
				else
				{
					mCells[sy * mNumCellInX + ((sx + iCount) % mNumCellInX)].mIsBlock = true;
					
				}
			}

			mSrcCellIndex = Int2(rand() % mNumCellInX, rand() % mNumCellInY);
			mDstCellIndex = Int2(rand() % mNumCellInX, rand() % mNumCellInY);

			GetCell(mSrcCellIndex)->mIsBlock = false;
			GetCell(mDstCellIndex)->mIsBlock = false;
		}
	}
	static Int2 GetLowestF(TArray<Int2>& in)
	{

	}
	static int  DistBettwen(Int2 from, Int2 to)
	{
		return HeuristicCostEstimate(from, to);
	}
	static int HeuristicCostEstimate(Int2 from, Int2 to)
	{
		Int2 d = from - to;
		return Sqrt(d.x * d.x + d.y * d.y);
	}



	bool OpenListIsEmpty() const 
	{
		return mOpenList.Length() == 0;
	}
	Int2 OpenListGrapLowestF()
	{
		int lowestF = INT_MAX;
		size_t lowestFIndex = 0;

		for (size_t iIndex = 0; iIndex < mOpenList.Length(); iIndex++)
		{
			int fs = GetCell(mOpenList[iIndex])->mFScore;
			if (fs <= lowestF)
			{
				lowestF = fs;
				lowestFIndex = iIndex;
			}
		}
		
		Int2 ret = mOpenList[lowestFIndex];
		GetCell(ret)->mIsInOpenList = false;
		GetCell(ret)->mIsInClosedList = true;
		mOpenList.RemoveAtSwap(lowestFIndex);
		return ret;
	}
	void OpenListAdd(Int2 cell)
	{
		mOpenList.Add(cell);
	}

	void ReconstructPath(Int2 current, AStarCell* cameFrom)
	{
		mResultPathPtr.RemoveAll();
		
		mResultPathPtr.Add(GetCell(mDstCellIndex));

		while (cameFrom)
		{
			mResultPathPtr.Add(cameFrom);
			cameFrom = cameFrom->mCameFrom;
		}
		mResultPathPtr.Reverse();
	}
	//////////////////////////////////////////////////////////////////////////
	void FindPath()
	{
		AStarCell* lastSet = nullptr;

		TArray<Int2> path;

		path.Add(mSrcCellIndex);

		{
			for (AStarCell& cell : mCells)
			{
				cell.mCameFrom = nullptr;
				cell.mIsInClosedList = false;
				cell.mIsInOpenList = false;
				cell.mFScore = INT_MAX;
				cell.mGScore = INT_MAX;
			}
		}

		TArray<Int2> cameFrom;

		mOpenList.Empty();
		mOpenList.Add(mSrcCellIndex);

		AStarCell* dstCell = GetCell(mDstCellIndex);
		AStarCell* srcCell = GetCell(mSrcCellIndex);
		//the cost of going from start to start is zero
		srcCell->mGScore = 0;
		//for the first node the value is completely heuristic
		srcCell->mFScore = HeuristicCostEstimate(mSrcCellIndex, mDstCellIndex);

		while (!OpenListIsEmpty())
		{
			Int2 currentIndex = OpenListGrapLowestF();
			AStarCell* currentCell = GetCell(currentIndex);

			if (currentCell == dstCell)
			{
				printf("path found\n");
				ReconstructPath(currentIndex, lastSet);
				return ;
			}


			const Int2 neighbousOffset[] =
			{
				Int2(1, 0), Int2(-1, 0), Int2(0, 1), Int2(0, -1)
			};

			for (unsigned iNeighbour = 0; iNeighbour < 4; iNeighbour++)
			{
				Int2 neighbourIndex = currentIndex + neighbousOffset[iNeighbour];

				if (neighbourIndex >= Int2(0, 0) && neighbourIndex < Int2(mNumCellInX, mNumCellInY))
				{
					AStarCell* neighbor = GetCell(neighbourIndex);
					
					if(neighbor->mIsBlock) 
						continue;

					if (neighbor->mIsInClosedList)
						continue;

					if (!neighbor->mIsInOpenList)
					{
						neighbor->mIsInOpenList = true;
						OpenListAdd(neighbourIndex);
					}

					// The distance from start to a neighbor
					int tentative_gScore = currentCell->mGScore + DistBettwen(currentIndex, neighbourIndex);
					if (tentative_gScore >= neighbor->mGScore)
						continue;	// This is not a better path.
					
					lastSet = currentCell;

					neighbor->mCameFrom = currentCell;
					neighbor->mGScore = tentative_gScore;
					neighbor->mFScore = neighbor->mGScore + HeuristicCostEstimate(neighbourIndex, mDstCellIndex);
				}
				
			}
		}

	/*
	function A*(start, goal)
	// The set of nodes already evaluated
	closedSet := {}

	// The set of currently discovered nodes that are not evaluated yet.
	// Initially, only the start node is known.
	openSet := {start}

	// For each node, which node it can most efficiently be reached from.
	// If a node can be reached from many nodes, cameFrom will eventually contain the
	// most efficient previous step.
	cameFrom := the empty map

	// For each node, the cost of getting from the start node to that node.
	gScore := map with default value of Infinity

	// The cost of going from start to start is zero.
	gScore[start] := 0

	// For each node, the total cost of getting from the start node to the goal
	// by passing by that node. That value is partly known, partly heuristic.
	fScore := map with default value of Infinity

	// For the first node, that value is completely heuristic.
	fScore[start] := heuristic_cost_estimate(start, goal)

	while openSet is not empty
		current := the node in openSet having the lowest fScore[] value
		if current = goal
			return reconstruct_path(cameFrom, current)

		openSet.Remove(current)
		closedSet.Add(current)

		for each neighbor of current
			if neighbor in closedSet
				continue		// Ignore the neighbor which is already evaluated.

			if neighbor not in openSet	// Discover a new node
				openSet.Add(neighbor)

			// The distance from start to a neighbor
			tentative_gScore := gScore[current] + dist_between(current, neighbor)
			if tentative_gScore >= gScore[neighbor]
				continue		// This is not a better path.

			// This path is the best until now. Record it!
			cameFrom[neighbor] := current
			gScore[neighbor] := tentative_gScore
			fScore[neighbor] := gScore[neighbor] + heuristic_cost_estimate(neighbor, goal)

	return failure

function reconstruct_path(cameFrom, current)
	total_path := [current]
	while current in cameFrom.Keys:
		current := cameFrom[current]
		total_path.append(current)
	return total_path
		*/
		
	}
};

AStarGrid gGrid;

void UDrawGrid(HDC hdc, unsigned wndW, unsigned wndH)
{
	unsigned ww = (wndW) / gGrid.mNumCellInX;
	unsigned hh = (wndH) / gGrid.mNumCellInY;

	HGDIOBJ redBrush = CreateSolidBrush(RGB(255, 0, 0));
	HGDIOBJ whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	HGDIOBJ greeBrush = CreateSolidBrush(RGB(0, 255, 0, ));
	HGDIOBJ yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	HGDIOBJ blueBrush = CreateSolidBrush(RGB(0, 0, 255));

	HGDIOBJ br = nullptr;

	{
		for (unsigned yy = 0; yy < gGrid.mNumCellInY; yy++)
		{
			for (unsigned xx = 0; xx < gGrid.mNumCellInX; xx++)
			{
				if (gGrid.GetCell(xx, yy)->mIsBlock)
				{
					SelectObject(hdc, redBrush);
					br = redBrush;
				}
				else
				{
					SelectObject(hdc, whiteBrush);
					br = whiteBrush;
				}

				//dst is green
				if (gGrid.mDstCellIndex == Int2(xx, yy))
				{
					br = greeBrush;
				}
				else if (gGrid.mSrcCellIndex == Int2(xx, yy))
				{
					br = yellowBrush;
				}



				RECT frect;
				frect.left = xx * ww;
				frect.top = yy * hh;
				frect.right = frect.left + ww;
				frect.bottom = frect.top + hh;

				//if (gGrid.mResultPath.HasElement(Int2(xx, yy)))
				size_t iPathElement = gGrid.mResultPathPtr.Find(gGrid.GetCell(Int2(xx, yy)));
				if (iPathElement != INVALID_INDEX)
				{
					//from bright to dark
					br = CreateSolidBrush(RGB(0,0, 255 * iPathElement / gGrid.mResultPathPtr.Length()));
					SelectObject(hdc, br);
					FillRect(hdc, &frect, (HBRUSH)br);
					DeleteObject(br);
					
				}
				else
				{
					FillRect(hdc, &frect, (HBRUSH)br);
				}
				
				//Rectangle(hdc, xx * ww, yy * hh, xx * ww + ww, yy * hh + hh);
			}
		}
	}
	
	DeleteObject(redBrush);
	DeleteObject(greeBrush);
	DeleteObject(whiteBrush);
	DeleteObject(yellowBrush);
	DeleteObject(blueBrush);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
	{
		HDC hdc = GetDC(hWnd);
		ReleaseDC(hWnd, hdc);

	};
	break;
	
	case WM_COMMAND:
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		RECT crect;
		GetClientRect(hWnd, &crect);
		
		unsigned wndW = crect.right - crect.left;
		unsigned wndH = crect.bottom - crect.top;
		
		UDrawGrid(hdc, wndW, wndH);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		unsigned xPos = GET_X_LPARAM(lParam);
		unsigned yPos = GET_Y_LPARAM(lParam);

		RECT crect;
		GetClientRect(hWnd, &crect);

		unsigned wndW = crect.right - crect.left;
		unsigned wndH = crect.bottom - crect.top;

		Int2 cellIndex = Int2(xPos / (wndW / gGrid.mNumCellInX), yPos / (wndH / gGrid.mNumCellInY));


		AStarCell& cell = *(gGrid.GetCell(cellIndex));
		if (message == WM_LBUTTONDOWN)
			cell.mIsBlock = true;
		else if(message == WM_RBUTTONDOWN)
			cell.mIsBlock = false;
		else if (message == WM_MBUTTONDOWN)
		{
			if ((wParam & MK_SHIFT) == MK_SHIFT)
				gGrid.mSrcCellIndex = cellIndex;
			else
				gGrid.mDstCellIndex = cellIndex;
		}
		InvalidateRect(gWnd, nullptr, true);

	}break;
	case WM_KEYDOWN:
	{
		if(wParam == 'F') //find Path
		{ 
			printf("Finding path\n");
			gGrid.FindPath();
		}
		InvalidateRect(hWnd, nullptr, true);
	}break;
	case WM_QUIT:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
class RenderAreW : public QWidget
{
public:
	AStarGrid mGrid;

	Int2 WidgetPosToCellIndex(QPoint p)
	{
		unsigned cellW = this->width() / mGrid.mNumCellInX;
		unsigned cellH = this->height() / mGrid.mNumCellInY;

		unsigned cellX = p.x() / cellW;
		unsigned cellY = p.y() / cellH;

		return Int2(cellX, cellY);
	}
	RenderAreW(QWidget* parent = nullptr) : QWidget(parent) 
	{
		mGrid.FillRnd();
		this->setMouseTracking(true);
		this->setBackgroundRole(QPalette::ColorRole::Base);
		this->setAutoFillBackground(true);
		this->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

		//ctx menu
		connect(this, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
			 auto gpos = this->mapToGlobal(pos);
			 QMenu* menu = new QMenu;
			 QAction* acSetDstHere = menu->addAction("Set Dst Here");

			 //set dst action
			 connect(acSetDstHere, &QAction::triggered, this, [this, pos](bool) {
				 this->mGrid.mDstCellIndex = this->WidgetPosToCellIndex(pos);
				 
				 this->update();
			 });

			 //set src action
			 QAction* acSetSrcHere = menu->addAction("Set Src Here");
			 connect(acSetSrcHere, &QAction::triggered, this, [this, pos](bool) {
				 this->mGrid.mSrcCellIndex = this->WidgetPosToCellIndex(pos);
				 this->update();
			 });
			 
			 //find path
			 QAction* acFindPath = menu->addAction("Find Path");
			 connect(acFindPath, &QAction::triggered, this, [this](bool) {
				 this->mGrid.FindPath();
			 });

			 menu->popup(gpos);
		});
	}

	virtual QSize sizeHint() const override
	{
		return QSize(400, 400);
	}


	virtual QSize minimumSizeHint() const override
	{
		return QSize(200, 200);
	}

protected:
	virtual void paintEvent(QPaintEvent *event) override
	{
		QPainter painter(this);
		painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
		painter.setPen(QColor(255, 0, 0));
		painter.drawLine(0, 0, 100, 100);
		painter.drawRect(0, 0, this->width() / 2, this->height() / 2);

		unsigned cellW = this->width() / mGrid.mNumCellInX;
		unsigned cellH = this->height() / mGrid.mNumCellInY;

		auto emptyBrush = QBrush(QColor(254, 254, 254));
		auto blockBrush = QBrush(QColor(255, 1, 1));
		auto srcBrsuh = QBrush(QColor(0, 255, 0));
		auto dstBrush = QBrush(QColor(0, 0, 255));

		QBrush brush;

		for (size_t y = 0; y < mGrid.mNumCellInY; y++)
		{
			for (size_t x = 0; x < mGrid.mNumCellInX; x++)
			{
				if (mGrid.GetCell(x, y)->mIsBlock)
					brush = blockBrush;
				else if (mGrid.mSrcCellIndex == Int2(x, y))
					brush = srcBrsuh;
				else if (mGrid.mDstCellIndex == Int2(x, y))
					brush = dstBrush;
				else
					brush = emptyBrush;
				
				if (mGrid.mResultPathPtr.HasElement(mGrid.GetCell(x, y)))
					brush = QBrush(QColor(255, 255, 0));

				painter.setBrush(blockBrush);
				painter.fillRect(x * cellW, y * cellH, cellW, cellH, brush);

			}
		}
	}


	virtual void mousePressEvent(QMouseEvent *event) override
	{

	}


	virtual void mouseReleaseEvent(QMouseEvent *event) override
	{
	}


	virtual void mouseDoubleClickEvent(QMouseEvent *event) override
	{
	}


	virtual void mouseMoveEvent(QMouseEvent *event) override
	{
		unsigned cellW = this->width() / mGrid.mNumCellInX;
		unsigned cellH = this->height() / mGrid.mNumCellInY;

		unsigned cellX = event->pos().x() / cellW;
		unsigned cellY = event->pos().y() / cellH;

		ULOG_MESSAGE("Cell Clicked % %", cellX, cellY);

		if (event->buttons() & Qt::MouseButton::LeftButton)
		{
			if(event->modifiers() == Qt::KeyboardModifier::ShiftModifier)
				mGrid.GetCell(cellX, cellY)->mIsBlock = false;
			else
				mGrid.GetCell(cellX, cellY)->mIsBlock = true;
		}
		this->update();
	}

};

int __main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow mainWnd;
	QToolBar* tb = mainWnd.addToolBar("ToolBar");
	tb->addAction("0");
	tb->addAction("1");
	tb->addAction("2");
	
	QVBoxLayout* vbox = new QVBoxLayout();
	mainWnd.setCentralWidget(new RenderAreW);
	mainWnd.centralWidget()->setLayout(vbox);
	mainWnd.showMaximized();
	
	return app.exec();

}