#pragma once
/** @file VariableInterpretation.h
 * @brief Fixed-size bindings and participation flags for lowercase variables.
 */
#include <stdexcept>
/// Whether ch is an ASCII lowercase letter between a and z.
bool isValidVariable(char ch);

/** @brief Map a-z to double values with explicit participation flags.
 * Adding a variable registers it; operator[] never inserts missing variables.
 * Participation counts are cached and recomputed after add/remove operations.
 */
class VariableInterpretation {
public:
	/// Shared empty binding set, used when folding variable-free subtrees.
	static const VariableInterpretation Empty;

	static constexpr size_t total_vars_count = 26;

	VariableInterpretation() = default;

	/// Access a registered variable; throws std::invalid_argument for invalid/missing names.
	double& operator[](char at);

	/// Read a registered variable; throws std::invalid_argument for invalid/missing names.
	double operator[](char at) const;

	/// Register or overwrite a-z with value; invalid names throw std::invalid_argument.
	void addVar(char ch, double value = 0.0);

	/** @brief Request removal of ch; invalid names throw std::invalid_argument.
	 * @bug Currently sets participation to true, so it registers rather than removes.
	 */
	void removeVar(char ch);

	/// Count active participation flags, updating the mutable cache when necessary.
	size_t getParticipatingCount() const;

	/// Query registration; invalid names throw std::invalid_argument.
	bool participates(char at) const;
private:
	mutable bool partCountChanged = true;

	mutable size_t partCount = 0;

	bool participatingVars[total_vars_count] = { false };

	double varValues[total_vars_count] = {0.0};

};
