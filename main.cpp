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

	vector<string> getEpsilonClosure(string state) // ��-closure ���ϴ� �Լ�
	{
		// epsilon arc�� ���� ���
		if (delta[{state, "epsilon"}][0] == "x")
		{
			return vector<string>(1, state); // �ڱ��ڽ� ����
		}

		// epsilon arc�� �ϳ��� ������
		set<string> s;
		s.insert(state);

		vector<string> nextStates = delta[{state, "epsilon"}]; // �Է»��¿��� epsilon arc�� ���� �� �� �ִ� ���� ��� ���� ��

		for (string nextState : nextStates)
		{
			s.insert(nextState);

			vector<string> temp = getEpsilonClosure(nextState); // ���� ���¿� ���� ��������� ��-closure�� �����ش�.

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

		cout << "---------------------------��ȯ �� reduced-DFA---------------------------" << endl;

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
					cout << "\t" << "��" << "\t";

				}
				else
				{
					cout << "\t" << "class" << groupNum << "\t";

				}

			}
			cout << endl;
		}

		cout << "���ۻ���: ";

		for (int i = 0; i < result.size(); i++)
		{
			if (find(result[i].begin(), result[i].end(), q0) != result[i].end())
			{
				cout << "class" << i << " ";
			}
		}
		cout << endl;

		cout << "�������: ";

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

	cout << "������ ������ �Է����ּ���. ";
	int numQ;
	cin >> numQ;

	bool start = true;

	cout << "���¸� �Է����ּ���. �������� �����ϸ�, ���� ó�� �Էµ� ���°� ���ۻ����Դϴ�. ";

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

	cout << "input symbol�� ������ �Է����ּ���. ";
	int numSigma;
	cin >> numSigma;

	cout << "input symbol�� �Է����ּ���. �������� �����մϴ�. ";

	for (int i = 0; i < numSigma; i++)
	{
		string character;
		cin >> character;
		eNFA.sigma.push_back(character);
	}

	cout << endl;
	cout << "��Ÿ�Լ��� �������ּ���. ��� �Է��� <���� ������ ����> <���� 1> <���� 2> ... <���� N>�� �������� �ԷµǾ�� �մϴ�." << endl;
	cout << "��, �������� x�� ǥ���մϴ�." << endl << endl;

	for (int i = 0; i < eNFA.Q.size(); i++)
	{
		for (int j = 0; j < eNFA.sigma.size(); j++)
		{
			cout << eNFA.Q[i] << "���� " << eNFA.sigma[j] << "�� ���� �� �� �ִ� ���´�? ";
			int numStates;
			cin >> numStates;

			for (int k = 0; k < numStates; k++)
			{
				string q;
				cin >> q;

				eNFA.delta[{eNFA.Q[i], eNFA.sigma[j]}].push_back(q);
			}
		}

		cout << eNFA.Q[i] << "���� epsilon�� ���� �� �� �ִ� ���´�? ";

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

	cout << "��������� ������ �Է����ּ���. ";
	int numFinals;
	cin >> numFinals;

	cout << "������¸� �Է����ּ���. �������� �����մϴ�. ";

	for (int i = 0; i < numFinals; i++)
	{
		string f;
		cin >> f;

		eNFA.F.push_back(f);
	}
	cout << endl;

	cout << endl;

	cout << "--------------------------�ʱ� ��-NFA----------------------------" << endl;

	for (int i = 0; i < eNFA.sigma.size(); i++)
	{
		if (eNFA.sigma[i] == "epsilon")
		{
			cout << "\t" << "��" << "\t";
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
				cout << "��";
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

	cout << "���ۻ���: " << eNFA.q0 << endl;
	cout << "�������: ";

	for (int i = 0; i < eNFA.F.size(); i++)
	{
		cout << eNFA.F[i] << " ";
	}
	cout << endl << endl;

	eNFA.sigma.pop_back();

	DFA dfa;

	map<string, string> m;

	char startCh = 'A';

	cout << "---------------------------��ȯ �� DFA---------------------------" << endl;

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
		vector<string> states = q.front(); // ť���� ���� �������� ��������
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
			cout << state; // ���� �������տ� ���ϴ� ��� ���¸� ����Ѵ�.
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

		for (string symbol : eNFA.sigma) // ��� input symbol�� ���Ͽ� ���� ���¸� ����ϴ� ����
		{
			set<string> s; // ���� ���¸� ���� �� �ߺ��Ǵ� ���¸� �������ֱ� ���� set

			for (string currentState : states) // �������տ� ���ϴ� ��� ���� ���¿� ���Ͽ� ���� ���¸� ���ϰ� ���������� union�� ����ϴ� ����
			{
				vector<string> nextStates = eNFA.delta[{currentState, symbol}]; // ���� ���������� ���Ѵ�.

				for (string nextState : nextStates)
				{
					if (nextState != "x") // "x"�� �������̹Ƿ� �����Ѵ�.
					{
						s.insert(nextState); // ���տ� �߰��� �ߺ� ����
					}
				}
			}

			vector<string> nextStates;

			for (string elem : s) // set�� vector�� ��ȯ
			{
				nextStates.push_back(elem);
			}

			// epsilon closure ���ϴ� ����
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

			// ���� ���� ���
			if (nextStates.empty())
			{
				cout << "��";

				dfa.delta[{str, symbol}] = "x";
			}

			else
			{
				string ch;
				for (string nextState : nextStates) // ���� ���� ���θ� ����Ѵ�.
				{
					if (nextState != "x") // �������� ����
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

			if (!visited[nextStates]) // ���� �湮���� ���� ���������̶�� ť�� �߰��ϰ�, �׷��� ������ �߰����� �ʴ´�.
			{
				q.push(nextStates);
				visited[nextStates] = true; // �̹� �湮�ߴ� ���������� ����صξ� ���ѷ����� �߻����� �ʵ��� �Ѵ�.
			}
		}
		cout << endl;
	}

	dfa.sigma = eNFA.sigma;

	sort(dfa.Q.begin(), dfa.Q.end());

	cout << "���ۻ���: " << dfa.q0 << endl;
	cout << "�������: ";

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