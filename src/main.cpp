#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

struct Page {
  int id;
  int logicalAddress;
  int bitR;
  bool operator==(const Page &other) const {
    return this->logicalAddress == other.logicalAddress;
  }
};

void clear_terminal() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

Page find_victim(vector<Page> &pages, int *last_page_id);
bool kill_victim(vector<Page> &pages, Page victim);
void print_pages(vector<Page>);
bool already_on_memory(vector<Page> &pages, Page page);

int main(int argc, char *argv[]) {
  vector<int> logicalAddresses = {3, 4, 5, 6, 10, 2, 3, 9, 45, 3, 2, 10, 200};
  vector<Page> pages = {};

  int pageTableSize = argc > 2 ? atoi(argv[2]) : 3;
  int *last_page_id = new int(-1);
  int counter = 0;

  for (int i = 0; i < logicalAddresses.size(); i++) {
    cout << "Inserindo: " << logicalAddresses[i] << endl;

    if (pages.size() < pageTableSize) {
      pages.push_back(Page{counter, logicalAddresses[i], 1});
      continue;
    }

    Page new_page = Page{counter, logicalAddresses[i], 1};

    if (!already_on_memory(pages, new_page)) {
      Page victim = find_victim(pages, last_page_id);
      kill_victim(pages, victim);
      pages.push_back(new_page);
    } else {
      auto it = find(pages.begin(), pages.end(), new_page);
      it->bitR = 1;
    }

    counter++;
    print_pages(pages);

    if (argc > 1)
      this_thread::sleep_for(chrono::milliseconds(atoi(argv[1])));
  }

  delete last_page_id;
}

void print_pages(vector<Page> pages) {
  cout << "____________________________________________" << endl;
  for (Page page : pages) {
    cout << "Endereço lógico: " << page.logicalAddress << " bitR: " << page.bitR
         << endl;
  }
  cout << "____________________________________________" << endl;
}

Page find_victim(vector<Page> &pages, int *last_page_id) {
  auto it = find_if(pages.begin(), pages.end(),
                    [&](Page &p) { return p.id == *last_page_id; });

  int start_index = (it != pages.end()) ? distance(pages.begin(), it) + 1 : 0;

  for (int i = 0; i < pages.size(); i++) {
    int index = (start_index + i) % pages.size();
    if (pages[index].bitR == 1) {
      pages[index].bitR = 0;
    } else {
      *last_page_id = pages[index].id;
      return pages[index];
    }
  }
  return find_victim(pages, last_page_id);
}

bool kill_victim(vector<Page> &pages, Page victim) {
  auto it = find(pages.begin(), pages.end(), victim);

  if (it != pages.end()) {
    pages.erase(it);
    return true;
  }

  return false;
}

bool already_on_memory(vector<Page> &pages, Page page) {
  auto it = find(pages.begin(), pages.end(), page);

  return it != pages.end();
}
