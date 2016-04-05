#include <iostream>
#include <string>

struct Student
{
  Student *next;
  std::string surname;
  double value;
};


void printList(Student *head)
{
  while (head != nullptr)
  {
    std::cout << head->value << ' ';
    head = head->next;
  }
  std::cout << std::endl;
}

Student* swap(Student *e1, Student *e2)
{
  e1->next = e2->next;
  e2->next = e1;
  return e2;
}


Student* sort(Student *head)
{
  if (head == nullptr) return nullptr;

  std::cout << "sort: "; printList(head);

  if (head->next != nullptr && head->value > head->next->value)
  {
    head = swap(head, head->next);
  }

  head->next = sort(head->next);

  if (head->next != nullptr && head->value > head->next->value)
  {
    head = swap(head, head->next);
    head->next = sort(head->next);
  }
  return head;
}


int main()
{
  Student s4 = {nullptr, "Foo", 2.0};
  Student s3 = {&s4, "Foo", 3.0};
  Student s2 = {&s3, "Bar", 4.0};
  Student s1 = {&s2, "Spam", 5.0};

  printList(&s1);
  Student *sorted = sort(&s1);
  printList(sorted);
}
