#pragma once
class Text
{
public:
	Text(void);
	~Text(void);

	void initText2D(const char*);
	void printText2D(const char*, int, int, int);
	void cleanupText2D();
};

