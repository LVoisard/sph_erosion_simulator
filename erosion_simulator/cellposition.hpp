struct CellPosition {
	int xLeft;
	int xRight;
	int yDown;
	int yUp;
	float xWeight;
	float yWeight;

public:
	CellPosition(float x, float y) {
		// Float casted to int are truncated towards 0.
		xLeft = (int)x;
		xRight = xLeft + 1;
		// This will act as the horizontal weight, indicating how close the point is to one side.
		xWeight = x - xLeft; // must be between [0, 1).

		yDown = (int)y;
		yUp = yDown + 1;
		// This will act as the vertical weight, indicating how close the point is to one side.
		yWeight = y - yDown;  // must be between [0, 1).
	}
};