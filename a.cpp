#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <unordered_map>
#include <utility>

using namespace std;

class Activity {
public:
  Activity (string &name, int danger_factor, int social_factor, int fitness_factor) {
    name_ = name;
    danger_factor_ = danger_factor;
    fitness_factor_ = fitness_factor;
  }
  string get_name() { return name_; }
  int get_danger_factor() { return danger_factor_; }
  int get_social_factor() { return social_factor_; }
  int get_fitness_factor() { return fitness_factor_; }
private:
  string name_;
  int danger_factor_;
  int social_factor_;
  int fitness_factor_;
};

class Human {
public:
  Human(int p_danger_factor = 1, int p_social_factor = 1, int p_fitness_factor = 1) {
    personal_danger_factor_ = p_danger_factor;
    personal_social_factor_ = p_social_factor;
    personal_fitness_factor_ = p_fitness_factor;
  }
  int get_danger_factor() { return danger_factor_ * personal_danger_factor_; }
  int get_social_factor() { return social_factor_ * personal_social_factor_; }
  int get_fitness_factor() { return fitness_factor_ * personal_fitness_factor_; }
  vector<Activity> &get_favorite_activities() { return favorite_activities_; }

protected:
  int danger_factor_;
  int social_factor_;
  int fitness_factor_;
  int personal_danger_factor_ = 1;
  int personal_social_factor_ = 1;
  int personal_fitness_factor_ = 1;
  vector<Activity> favorite_activities_;
};

class Male : public Human {
public:
  Male(int p_danger_factor = 1, int p_social_factor = 1, int p_fitness_factor = 1):
    Human(p_danger_factor, p_social_factor, p_fitness_factor)
  {
    danger_factor_ = 100;
    social_factor_ = 50;
    fitness_factor_ = 70;
  }

};

class Female : public Human {
public:
  Female(int p_danger_factor = 1, int p_social_factor = 1, int p_fitness_factor = 1):
    Human(p_danger_factor, p_social_factor, p_fitness_factor)
  {
    danger_factor_ = -100;
    social_factor_ = 100;
    fitness_factor_ = 50;
  }
};

template <typename T>
class GenderActivityComparison {
public:
  GenderActivityComparison(T t) : t_(t) {}
  bool operator()(Activity &a1, Activity &a2)
  {
    int a1_liking = a1.get_danger_factor() * t_.get_danger_factor() +
      a1.get_social_factor() * t_.get_social_factor() +
      a1.get_fitness_factor() * t_.get_fitness_factor();

    int a2_liking = a2.get_danger_factor() * t_.get_danger_factor() +
      a2.get_social_factor() * t_.get_social_factor() +
      a2.get_fitness_factor() * t_.get_fitness_factor();

    return a1_liking > a2_liking;
  }
private:
  T t_;
};

vector<Activity> get_activities(char *file_name)
{
  vector<Activity> result;

  ifstream ifs(file_name);
  if (!ifs.is_open()) {
    cout << "Error: Coudn't open file \"" << file_name << "\"" << endl;
    return result;
  }

  string name;
  int danger, social, fitness;
  while (!ifs.eof()) {
    ifs >> name >> danger >> social >> fitness;
    result.push_back(Activity(name, danger, social, fitness));
  }
  result.pop_back();

  return result;
}

template <typename T>
vector<T> create_humans(int n, vector<Activity> &activities)
{
  vector<T> result;

  for (int i = 0; i < n; i++) {
    result.push_back(T(rand() % 100, rand() % 100, rand() % 100 ));
    for (auto activity : activities)
      result[i].get_favorite_activities().push_back(activity);

    sort(result[i].get_favorite_activities().begin(),
         result[i].get_favorite_activities().end(),
         GenderActivityComparison<T>(result[i]));
  }

  return result;
}

class MyCompare {
public:
  bool operator()(const pair<string, int> &a, const pair<string, int> &b)
  {
    return a.second > b.second;
  }
};

int main(int argc, char **argv)
{
  if (argc < 2) {
    cout << "Error: no input files\n" << "Usage: " << argv[0] << " <activities_file>\n";
    exit(1);
  }

  vector<Activity> activities = get_activities(argv[1]);
  vector<Male> males = create_humans<Male>(1000, activities);
  vector<Female> females = create_humans<Female>(1000, activities);

  if (!activities.size()) {
    cout << "I can't do much without activities. suicide!";
    exit(1);
  }

  unordered_map<string, int> boys_top_count, boys_bottom_count;
  for (auto male : males) {
    auto fav_activities = male.get_favorite_activities();
    string top = fav_activities[0].get_name();
    string bottom = fav_activities[fav_activities.size() - 1].get_name();

    boys_top_count[top]++;
    boys_bottom_count[bottom]++;
  }

  set<pair<string, int>, MyCompare> boys_top_activities(boys_top_count.begin(), boys_top_count.end());
  set<pair<string, int>, MyCompare> boys_bottom_activities(boys_bottom_count.begin(), boys_bottom_count.end());

  cout << "\nBoys top activities:\n";
  for (auto top : boys_top_activities)
    cout << top.first << " [choosed by " << top.second << " males]\n";

  cout << "\nBoys bottom activities:\n";
  for (auto top : boys_bottom_activities)
    cout << top.first << " [choosed by " << top.second << " males]\n";


  unordered_map<string, int> girls_top_count, girls_bottom_count;
  for (auto female : females) {
    auto fav_activities = female.get_favorite_activities();
    string top = fav_activities[0].get_name();
    string bottom = fav_activities[fav_activities.size() - 1].get_name();

    girls_top_count[top]++;
    girls_bottom_count[bottom]++;
  }

  set<pair<string, int>, MyCompare> girls_top_activities(girls_top_count.begin(), girls_top_count.end());
  set<pair<string, int>, MyCompare> girls_bottom_activities(girls_bottom_count.begin(), girls_bottom_count.end());

  cout << "\nGirls top activities:\n";
  for (auto top : girls_top_activities)
    cout << top.first << " [choosed by " << top.second << " females]\n";

  cout << "\nGirls bottom activities:\n";
  for (auto top : girls_bottom_activities)
    cout << top.first << " [choosed by " << top.second << " females]\n";

  return 0;
}
