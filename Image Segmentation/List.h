#ifndef LIST
#define LIST
#include<cstdlib>
#include<iostream>
using namespace std;

template<typename E>
struct Link
{
	E item;
	Link* next;
	Link() :next(nullptr){}
	Link(const E& it, Link* in = nullptr)
	{
		item = it;
		next = in;
	}
};

//类中不做断言检查，必须小心谨慎地使用
template<typename E, typename Compare>
class LList
{
	Link<E>* head, *pos, *tail;
	int length;

public:
	LList()
	{
		head = pos = tail = new Link<E>;
		length = 0;
	}
	~LList()
	{
		clear();
		delete head;
	}
	void clear()
	{
		pos = head->next;;
		while (pos != nullptr)
		{
			tail = pos->next;
			delete pos;
			pos = tail;
		}
		pos = tail = head;
		length = 0;
	}
	void insert(const E& it)
	{
		if (length == 0)
		{
			tail = tail->next = new Link<E>(it);
			++length;
			return;
		}
		else if ( pos==tail || !Compare::prior(getValue(), it))
		{
			pos = head;
		}

		while (pos!=tail && Compare::prior(getValue(), it))
		{
			pos = pos->next;
		}
		//insert here
		pos->next = new Link<E>(it, pos->next);
		++length;
		if (pos == tail)
			tail = pos->next;
	}
	E& getValue() const
	{
		return pos->next->item;
	}

	void removeFirst()
	{
		if (tail == head->next)
		{
			pos = tail = head;
		}
		else if(pos == head->next)
		{
			pos = pos->next;
		}

		auto tod = head->next;
		head->next = tod->next;
		delete tod;
		--length;
	}

	E first() const { return head->next->item;  }

	int size() const { return length;  }
	bool empty()const { return (head->next == nullptr); }
	bool isContain(const E& it) const
	{
		auto temp = head->next;
		while (temp != nullptr)
		{
			if (temp->item == it) return true;
			else temp = temp->next;
		}
		return false;
	}
	void remove(const E&it)
	{
		if(pos == tail || !Compare::prior(getValue(), it))
			pos = head;
		while (pos->next != nullptr)
		{
			if (pos->next->item == it) break;
			else pos = pos->next;
		}
		if (pos != tail)
		{
			auto tod = pos->next;
			pos->next = tod->next;
			delete tod;
			--length;
		}
		if (pos->next == nullptr)
		{
			tail = pos;
		}
	}

	void show()
	{
		auto temp = head->next;
		while (temp != nullptr)
		{
			cout << temp->item << endl;
			temp = temp->next;
		}
	}
	
};

#endif
