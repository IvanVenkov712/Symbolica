#pragma once
#include <stdexcept>
bool isValidVariable(char ch);

class VariableInterpretation {
public:
	static const VariableInterpretation Empty;

	static constexpr size_t total_vars_count = 26;

	VariableInterpretation() = default;

	double& operator[](char at);

	double operator[](char at) const;

	void addVar(char ch, double value = 0.0);

	void removeVar(char ch);

	size_t getParticipatingCount() const;

	bool participates(char at) const;
private:
	mutable bool partCountChanged = true;

	mutable size_t partCount = 0;

	bool participatingVars[total_vars_count] = { false };

	double varValues[total_vars_count] = {0.0};

};