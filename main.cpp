#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

class EpsilonNFA
{
public:
	vector<string> Q;
	vector<string> sigma;
	map<pair<string, string>, vector<string>> delta;
	string q0;
	vector<string> F;

	vector<string> getEpsilonClosure(string state) // ε-closure 구하는 함수
	{
		// epsilon arc가 없는 경우
		if (delta[{state, "epsilon"}][0] == "x")
		{
			return vector<string>(1, state); // 자기자신 리턴
		}

		// epsilon arc가 하나라도 있으면
		set<string> s;
		s.insert(state);

		vector<string> nextStates = delta[{state, "epsilon"}]; // 입력상태에서 epsilon arc를 통해 갈 수 있는 상태 모두 구한 후

		for (string nextState : nextStates)
		{
			s.insert(nextState);

			vector<string> temp = getEpsilonClosure(nextState); // 다음 상태에 대해 재귀적으로 ε-closure를 구해준다.

			for (string elem : temp)
			{
				s.insert(elem);
			}
		}

		vector<string> result;

		for (string elem : s)
		{
			result.push_back(elem);
		}

		return result;
	}
};

class DFA
{
public:
	vector<string> Q;
	vector<string> sigma;
	map<pair<string, string>, string> delta;
	string q0;
	vector<string> F;

	void minimize()
	{
		vector<vector<string>> groups(2);

		for (string state : Q)
		{
			if (find(F.begin(), F.end(), state) != F.end())
			{
				groups[0].push_back(state);
			}
			else
			{
				groups[1].push_back(state);
			}
		}

		vector<vector<string>> result;

		while (true)
		{
			bool flag = false;

			vector<vector<string>> newGroups;
			int idx = 0;

			for (vector<string> group : groups)
			{
				bool changed = false;
				int emptysetIndex = 0;

				for (int k = 0; k < sigma.size(); k++)
				{
					string symbol = sigma[k];

					map<int, vector<string>> m;

					for (string state : group)
					{
						string nextState = delta[{state, symbol}];

						if (nextState == "x")
						{
							m[500 + emptysetIndex].push_back(state);
							emptysetIndex++;

						}

						else
						{
							int groupNum;

							for (int i = 0; i < groups.size(); i++)
							{
								if (find(groups[i].begin(), groups[i].end(), nextState) != groups[i].end())
								{
									groupNum = i;
									break;
								}
							}

							m[groupNum].push_back(state);
						}


					}

					int cnt = 0;

					for (auto itr = m.begin(); itr != m.end(); itr++)
					{
						cnt++;
					}

					if (cnt == 1 && k == sigma.size() - 1)
					{
						for (auto itr = m.begin(); itr != m.end(); itr++)
						{
							newGroups.push_back(vector<string>());

							for (auto elem : itr->second)
							{
								newGroups[idx].push_back(elem);
							}
							idx++;
						}
					}

					else if (cnt >= 2)
					{
						for (auto itr = m.begin(); itr != m.end(); itr++)
						{
							newGroups.push_back(vector<string>());

							for (auto elem : itr->second)
							{
								newGroups[idx].push_back(elem);
							}
							idx++;
						}
						changed = true;
						flag = true;
					}

					if (changed)
					{
						break;
					}
				}

			}

			result = newGroups;

			groups.clear();
			groups = result;

			if (!flag)
			{
				break;
			}

		}

		cout << "---------------------------변환 후 reduced-DFA---------------------------" << endl;

		cout << "\t";
		for (string symbol : sigma)
		{
			cout << "\t" << symbol << "\t";
		}
		cout << endl;

		for (int i = 0; i < result.size(); i++)
		{
			vector<string> group = result[i];

			cout << "class" << i << "\t";

			for (string symbol : sigma)
			{
				string current = group[0];
				string nextState = delta[{current, symbol}];


				int groupNum;

				bool found = false;

				for (int i = 0; i < result.size(); i++)
				{
					if (find(result[i].begin(), result[i].end(), nextState) != result[i].end())
					{
						groupNum = i;
						found = true;
						break;
					}
				}
				if (!found)
				{
					cout << "\t" << "Ø" << "\t";

				}
				else
				{
					cout << "\t" << "class" << groupNum << "\t";

				}

			}
			cout << endl;
		}

		cout << "시작상태: ";

		for (int i = 0; i < result.size(); i++)
		{
			if (find(result[i].begin(), result[i].end(), q0) != result[i].end())
			{
				cout << "class" << i << " ";
			}
		}
		cout << endl;

		cout << "종결상태: ";

		for (int i = 0; i < result.size(); i++)
		{
			for (string finalState : F)
			{
				if (find(result[i].begin(), result[i].end(), finalState) != result[i].end())
				{
					cout << "class" << i << " ";
					break;
				}
			}

		}
		cout << endl;
	}
};

int main()
{
	EpsilonNFA eNFA;

	cout << "상태의 개수를 입력해주세요. ";
	int numQ;
	cin >> numQ;

	bool start = true;

	cout << "상태를 입력해주세요. 공백으로 구분하며, 가장 처음 입력된 상태가 시작상태입니다. ";

	for (int i = 0; i < numQ; i++)
	{
		string q;
		cin >> q;

		if (start)
		{
			eNFA.q0 = q;
			start = false;
		}

		eNFA.Q.push_back(q);
	}

	cout << endl;

	cout << "input symbol의 개수를 입력해주세요. ";
	int numSigma;
	cin >> numSigma;

	cout << "input symbol를 입력해주세요. 공백으로 구분합니다. ";

	for (int i = 0; i < numSigma; i++)
	{
		string character;
		cin >> character;
		eNFA.sigma.push_back(character);
	}

	cout << endl;
	cout << "델타함수를 정의해주세요. 모든 입력은 <다음 상태의 개수> <상태 1> <상태 2> ... <상태 N>의 형식으로 입력되어야 합니다." << endl;
	cout << "단, 공집합은 x로 표현합니다." << endl << endl;

	for (int i = 0; i < eNFA.Q.size(); i++)
	{
		for (int j = 0; j < eNFA.sigma.size(); j++)
		{
			cout << eNFA.Q[i] << "에서 " << eNFA.sigma[j] << "를 보고 갈 수 있는 상태는? ";
			int numStates;
			cin >> numStates;

			for (int k = 0; k < numStates; k++)
			{
				string q;
				cin >> q;

				eNFA.delta[{eNFA.Q[i], eNFA.sigma[j]}].push_back(q);
			}
		}

		cout << eNFA.Q[i] << "에서 epsilon을 보고 갈 수 있는 상태는? ";

		int numStates;
		cin >> numStates;

		for (int k = 0; k < numStates; k++)
		{
			string q;
			cin >> q;

			eNFA.delta[{eNFA.Q[i], "epsilon"}].push_back(q);
		}
		cout << endl;
	}

	eNFA.sigma.push_back("epsilon");

	cout << "종결상태의 개수를 입력해주세요. ";
	int numFinals;
	cin >> numFinals;

	cout << "종결상태를 입력해주세요. 공백으로 구분합니다. ";

	for (int i = 0; i < numFinals; i++)
	{
		string f;
		cin >> f;

		eNFA.F.push_back(f);
	}
	cout << endl;

	cout << endl;

	cout << "--------------------------초기 ε-NFA----------------------------" << endl;

	for (int i = 0; i < eNFA.sigma.size(); i++)
	{
		if (eNFA.sigma[i] == "epsilon")
		{
			cout << "\t" << "ε" << "\t";
		}
		else
		{
			cout << "\t" << eNFA.sigma[i] << "\t";
		}
	}

	cout << endl;

	for (string state : eNFA.Q)
	{
		cout << state;
		cout << "\t";

		for (string symbol : eNFA.sigma)
		{
			int cnt = 0;

			vector<string> nextStates = eNFA.delta[{state, symbol}];

			if (nextStates[0] == "x")
			{
				cout << "Ø";
			}

			else
			{
				cout << "{";
				for (int i = 0; i < nextStates.size(); i++)
				{
					if (i == nextStates.size() - 1)
					{
						cout << nextStates[i];
						cnt += nextStates[i].size();
					}
					else
					{
						cout << nextStates[i] << ", ";
						cnt += nextStates[i].size() + 2;
					}
				}
				cout << "}";
			}

			if (cnt >= 6)
			{
				cout << "\t";

			}
			else
			{
				cout << "\t\t";
			}
		}

		cout << endl;
	}

	cout << "시작상태: " << eNFA.q0 << endl;
	cout << "종결상태: ";

	for (int i = 0; i < eNFA.F.size(); i++)
	{
		cout << eNFA.F[i] << " ";
	}
	cout << endl << endl;

	eNFA.sigma.pop_back();

	DFA dfa;

	map<string, string> m;

	char startCh = 'A';

	cout << "---------------------------변환 후 DFA---------------------------" << endl;

	for (int i = 0; i < eNFA.sigma.size(); i++)
	{
		cout << "\t" << eNFA.sigma[i] << "\t";
	}
	cout << endl;

	queue<vector<string>> q;

	vector<string> v;
	v.push_back(eNFA.q0);

	q.push(v);
	map<vector<string>, bool> visited;

	visited[vector<string>(1, eNFA.q0)] = true;

	bool firstLoop = true;

	while (!q.empty())
	{
		vector<string> states = q.front(); // 큐에서 현재 상태집합 가져오기
		q.pop();

		if (states.empty())
		{
			continue;
		}

		set<string> tempSet2;

		for (string elem : states)
		{
			vector<string> tempVec = eNFA.getEpsilonClosure(elem);

			for (string closure : tempVec)
			{
				tempSet2.insert(closure);
			}
		}

		states.clear();

		for (string elem : tempSet2)
		{
			states.push_back(elem);
		}

		string str;

		bool found = false;
		bool foundInitial = false;

		for (string state : states)
		{
			cout << state; // 꺼낸 상태집합에 속하는 모든 상태를 출력한다.
			str += state;

			if (find(eNFA.F.begin(), eNFA.F.end(), state) != eNFA.F.end())
			{
				found = true;
			}

			if (firstLoop)
			{
				if (state == eNFA.q0)
				{
					foundInitial = true;
				}

				firstLoop = false;
			}
		}

		if (found)
		{
			dfa.F.push_back(str);
		}

		if (foundInitial)
		{
			dfa.q0 = str;
		}

		m[str] = startCh;
		startCh++;

		cout << "\t";

		for (string symbol : eNFA.sigma) // 모든 input symbol에 대하여 다음 상태를 출력하는 로직
		{
			set<string> s; // 다음 상태를 구할 때 중복되는 상태를 제거해주기 위한 set

			for (string currentState : states) // 상태집합에 속하는 모든 단일 상태에 대하여 다음 상태를 구하고 최종적으로 union을 계산하는 로직
			{
				vector<string> nextStates = eNFA.delta[{currentState, symbol}]; // 다음 상태집합을 구한다.

				for (string nextState : nextStates)
				{
					if (nextState != "x") // "x"는 공집합이므로 무시한다.
					{
						s.insert(nextState); // 집합에 추가해 중복 제거
					}
				}
			}

			vector<string> nextStates;

			for (string elem : s) // set을 vector로 변환
			{
				nextStates.push_back(elem);
			}

			// epsilon closure 구하는 로직
			set<string> tempSet;

			for (string elem : nextStates)
			{
				vector<string> tempVec = eNFA.getEpsilonClosure(elem);

				for (string closure : tempVec)
				{
					tempSet.insert(closure);
				}
			}

			nextStates.clear();

			for (string elem : tempSet)
			{
				nextStates.push_back(elem);
			}

			// 다음 상태 출력
			if (nextStates.empty())
			{
				cout << "Ø";

				dfa.delta[{str, symbol}] = "x";
			}

			else
			{
				string ch;
				for (string nextState : nextStates) // 다음 상태 전부를 출력한다.
				{
					if (nextState != "x") // 공집합은 제외
					{
						cout << nextState;
						ch += nextState;
					}
				}

				dfa.delta[{str, symbol}] = ch;
			}

			if (find(dfa.Q.begin(), dfa.Q.end(), str) == dfa.Q.end())
			{
				dfa.Q.push_back(str);
			}

			cout << "\t\t";

			if (!visited[nextStates]) // 아직 방문하지 않은 상태집합이라면 큐에 추가하고, 그렇지 않으면 추가하지 않는다.
			{
				q.push(nextStates);
				visited[nextStates] = true; // 이미 방문했던 상태집합을 기록해두어 무한루프가 발생하지 않도록 한다.
			}
		}
		cout << endl;
	}

	dfa.sigma = eNFA.sigma;

	sort(dfa.Q.begin(), dfa.Q.end());

	cout << "시작상태: " << dfa.q0 << endl;
	cout << "종결상태: ";

	for (string state : dfa.F)
	{
		cout << state << " ";
	}
	cout << endl;

	m["x"] = "x";

	cout << endl;

	dfa.minimize();

	return 0;
}