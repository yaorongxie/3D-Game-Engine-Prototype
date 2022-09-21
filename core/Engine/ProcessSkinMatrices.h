#pragma once
#include "Job.h"

class Character;

class ProcessSkinMatrices :	public Job
{
	public:
		ProcessSkinMatrices(Character* pCharacter);
		~ProcessSkinMatrices();
		void DoIt() override;

	private:
		Character* mCharacter;
};

