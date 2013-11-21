#pragma once

class VSText
{
	public:
		VSText();
		~VSText(void);
		void BuildFont(void);
		void KillFont(void);
		void glPrint(const char*, ...);
};

