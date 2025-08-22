#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <dirent.h>
#include "main.h"

int main() {
  main_menu();
  return 0;
}

void clear_screen() {
  std::cout << "\033[2J\033[1;1H";
}

void load_members() {
  std::string configDir = std::string(getenv("HOME")) + "/.config/rock_splits";
  std::string mkdirCmd = "mkdir -p \"" + configDir + "\"";
  system(mkdirCmd.c_str());
  
  std::string membersFile = configDir + "/members.txt";
  house_members.clear();

  std::ifstream file(membersFile);

  if (file.is_open()) {
    std::string member;
    while (std::getline(file, member)) {
      if (!member.empty()) {
        house_members.push_back(member);
      }
    }
    file.close();
  } else {
    house_members = {};
    save_members();
  }
}

void save_members() {
  std::string configDir = std::string(getenv("HOME")) + "/.config/rock_splits";
  std::string membersFile = configDir + "/members.txt";
  
  std::ofstream file(membersFile);
  
  if (file.is_open()) {
    for (const auto& member : house_members) {
      file << member << std::endl;
    }
    file.close();
  } else {
    std::cerr << "Error: Could not save members to file at " << membersFile << std::endl;
  }
}

void manage_members() {
  clear_screen();
  std::cout << "Member Management\n";
  std::cout << "-----------------\n\n";
  
  std::cout << "Current members:\n";
  for (size_t i = 0; i < house_members.size(); ++i) {
    std::cout << i + 1 << ". " << house_members[i] << std::endl;
  }
  std::cout << "\n";
  
  std::cout << "1. Add a member\n";
  std::cout << "2. Remove a member\n";
  std::cout << "3. Return to main menu\n";
  std::cout << "Choice: ";
  
  int choice;
  std::cin >> choice;
  std::cin.ignore();
  
  switch (choice) {
    case 1: {
      std::cout << "Enter new member name: ";
      std::string newMember;
      std::getline(std::cin, newMember);
      
      if (!newMember.empty()) {
        house_members.push_back(newMember);
        save_members();
        std::cout << "Added " << newMember << " to members.\n";
      }
      break;
    }
    case 2: {
      std::cout << "Enter the number of the member to remove: ";
      int memberNum;
      std::cin >> memberNum;
      
      if (memberNum > 0 && memberNum <= static_cast<int>(house_members.size())) {
        std::string removedMember = house_members[memberNum - 1];
        house_members.erase(house_members.begin() + (memberNum - 1));
        save_members();
        std::cout << "Removed " << removedMember << " from members.\n";
      } else {
        std::cout << "Invalid member number.\n";
      }
      break;
    }
    case 3:
      return;
    default:
      std::cout << "Invalid choice.\n";
      break;
  }

  std::cout << "\nPress Enter to continue...";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  manage_members();
}

void main_menu () {
  load_members();

  current_time_and_date = get_current_time_and_date();
  clear_screen();
  std::cout << "Current time and date: " << current_time_and_date << std::endl;
  std::cout << "1. Start new split" << std::endl;
  std::cout << "2. View previous splits" << std::endl;
  std::cout << "3. Manage house members" << std::endl;
  std::cout << "4. Exit" << std::endl;
  int choice;
  std::cin >> choice;
  switch (choice) {
    case 1:
      start_new_split();
      break;
    case 2:
      view_previous_splits();
      break;
    case 3:
      manage_members();
      main_menu();
      break;
    case 4:
      std::cout << "Exiting program..." << std::endl;
      return;
    default:
      std::cout << "Invalid choice. Please try again." << std::endl;
      main_menu();
      break;
  }
}

void start_new_split() {
  clear_screen();

  float input = 0.0f;
  std::cout << "Enter gas total: ";
  std::cin >> input;
  gas_total += input;

  std::cout << "Enter electric total: ";
  std::cin >> input;
  electric_total += input;

  if (electric_total > 0.0f) {
    std::cout << "Enter electric static charges (comma separated list): ";
    std::string input_str;
    std::cin.ignore();
    std::getline(std::cin, input_str);
    electric_static_charges = split_comma_separated_string(input_str);
    for (float charge : electric_static_charges) {
      electric_static += charge;
    }
  }

  std::cout << "Enter water total: ";
  std::cin >> input;
  water_total += input;

  if (water_total > 0.0f) {
    std::cout << "Enter water static charges (comma separated list): ";
    std::string input_str;
    std::cin.ignore();
    std::getline(std::cin, input_str);
    water_static_charges = split_comma_separated_string(input_str);
    for (float charge : water_static_charges) {
      water_static += charge;
    }
  }

  std::cout << "Enter internet total: ";
  std::cin >> input;
  internet_total += input;

  clear_screen();

  int index = 0;
  for (const auto& member : house_members) {
    std::cout << " " << index << ": " << member << std::endl;
    index++;
  }

  std::cout << std::endl;
  std::cout << "Enter a comma separated list of member IDs for all members currently living in the house, or leave empty if all members are currently in the house: ";
  std::string test;
  std::cin.ignore();
  std::getline(std::cin, test);
  std::vector<std::string> living_members;
  
  if (test.empty()) {
    living_members = house_members;
  } else {
    std::istringstream iss(test);
    std::string member_id;
    while (std::getline(iss, member_id, ',')) {
      member_id.erase(0, member_id.find_first_not_of(" \t"));
      member_id.erase(member_id.find_last_not_of(" \t") + 1);
      if (!member_id.empty()) {
        try {
          int id = std::stoi(member_id);
          if (id >= 0 && id < house_members.size()) {
            living_members.push_back(house_members[id]);
          } else {
            std::cout << "Warning: Invalid member ID " << id << " (out of range). Skipping." << std::endl;
          }
        } catch (const std::invalid_argument& e) {
          std::cout << "Warning: Invalid member ID '" << member_id << "'. Skipping." << std::endl;
        } catch (const std::out_of_range& e) {
          std::cout << "Warning: Member ID out of range. Skipping." << std::endl;
        }
      }
    }
  }

  float split_static_total = (electric_static + water_static + internet_total) / house_members.size();
  float split_variable_total = (gas_total + (electric_total - electric_static) + (water_total - water_static)) / living_members.size();

  for (const auto& member : house_members) {
    split_map[member] = split_static_total;
    if (std::find(living_members.begin(), living_members.end(), member) != living_members.end()) {
      split_map[member] += split_variable_total;
    }
  }
  generate_report();
}

void generate_report() {
  clear_screen();
  std::cout << std::fixed << std::setprecision(2);
  
  std::cout << "Bill totals:" << std::endl;
  std::cout << "Gas: $" << gas_total << std::endl;
  std::cout << "Electric: $" << electric_total << std::endl;
  std::cout << "Water: $" << water_total << std::endl;
  std::cout << "Internet: $" << internet_total << std::endl;
  std::cout << std::endl;

  std::cout << "Total: $" << (gas_total + electric_total + water_total + internet_total) << std::endl;
  std::cout << std::endl;

  std::cout << "Base Charges:" << std::endl;
  std::cout << "Electric: $" << electric_static << std::endl;
  std::cout << "Water: $" << water_static << std::endl;
  std::cout << "Internet: $" << internet_total << std::endl;
  std::cout << std::endl;

  std::cout << "Base Charges Total: $" << (electric_static + water_static + internet_total) << std::endl;
  std::cout << std::endl;
  
  for (const auto& [member, amount] : split_map) {
    std::cout << "Member: " << member << ", Amount: $" << amount << std::endl;
  }

  std::string reportsDir = std::string(getenv("HOME")) + "/.cache/rock_splits/reports";
  std::string mkdirCmd = "mkdir -p \"" + reportsDir + "\"";
  int dirCreated = system(mkdirCmd.c_str());

  std::string filename = "rock_splits_report_" + current_time_and_date.substr(0, current_time_and_date.length() - 1) + ".txt";
  std::replace(filename.begin(), filename.end(), ' ', '_');
  std::replace(filename.begin(), filename.end(), ':', '-');
  
  std::string fullPath = reportsDir + "/" + filename;
  std::ofstream reportFile(fullPath);
  
  if (reportFile.is_open()) {
    reportFile << "==================================================" << std::endl;
    reportFile << "           ROCK SPLITS DETAILED REPORT            " << std::endl;
    reportFile << "==================================================" << std::endl;
    reportFile << "Generated: " << current_time_and_date << std::endl;
    reportFile << "==================================================" << std::endl << std::endl;
    
    reportFile << "BILL SUMMARY" << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << std::fixed << std::setprecision(2);
    reportFile << "Gas Bill:       $" << std::setw(10) << gas_total << std::endl;
    reportFile << "Electric Bill:  $" << std::setw(10) << electric_total << std::endl;
    reportFile << "Water Bill:     $" << std::setw(10) << water_total << std::endl;
    reportFile << "Internet Bill:  $" << std::setw(10) << internet_total << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "TOTAL BILLS:    $" << std::setw(10) << (gas_total + electric_total + water_total + internet_total) << std::endl << std::endl;

    reportFile << "BASE CHARGES BREAKDOWN" << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "Electric base charges: $" << std::setw(10) << electric_static << std::endl;
    
    if (!electric_static_charges.empty()) {
      reportFile << "  Breakdown of electric static charges:" << std::endl;
      for (size_t i = 0; i < electric_static_charges.size(); ++i) {
        reportFile << "    Charge " << i+1 << ": $" << std::setw(10) << electric_static_charges[i] << std::endl;
      }
    }
    
    reportFile << "Water base charges:    $" << std::setw(10) << water_static << std::endl;
    
    if (!water_static_charges.empty()) {
      reportFile << "  Breakdown of water static charges:" << std::endl;
      for (size_t i = 0; i < water_static_charges.size(); ++i) {
        reportFile << "    Charge " << i+1 << ": $" << std::setw(10) << water_static_charges[i] << std::endl;
      }
    }
    
    reportFile << "Internet (fixed):      $" << std::setw(10) << internet_total << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "TOTAL BASE CHARGES:    $" << std::setw(10) << (electric_static + water_static + internet_total) << std::endl << std::endl;

    float total_variable = (gas_total + (electric_total - electric_static) + (water_total - water_static));
    reportFile << "VARIABLE CHARGES BREAKDOWN" << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "Gas (all variable):        $" << std::setw(10) << gas_total << std::endl;
    reportFile << "Electric (variable part):  $" << std::setw(10) << (electric_total - electric_static) << std::endl;
    reportFile << "Water (variable part):     $" << std::setw(10) << (water_total - water_static) << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "TOTAL VARIABLE CHARGES:    $" << std::setw(10) << total_variable << std::endl << std::endl;
    
    reportFile << "SPLIT CALCULATION EXPLANATION" << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    reportFile << "Total house members: " << house_members.size() << std::endl;
    
    std::vector<std::string> living_members;
    for (const auto& member : house_members) {
      if (split_map[member] > (electric_static + water_static + internet_total) / house_members.size()) {
        living_members.push_back(member);
      }
    }
    
    reportFile << "Members currently living in house: " << living_members.size() << std::endl;
    reportFile << "  ";
    for (size_t i = 0; i < living_members.size(); ++i) {
      reportFile << living_members[i];
      if (i < living_members.size() - 1) reportFile << ", ";
    }
    reportFile << std::endl << std::endl;
    
    float split_static_total = (electric_static + water_static + internet_total) / house_members.size();
    float split_variable_total = total_variable / living_members.size();
    
    reportFile << "Split calculation method:" << std::endl;
    reportFile << "1. Base charges (electric static + water static + internet) divided equally among ALL members" << std::endl;
    reportFile << "   $" << (electric_static + water_static + internet_total) << " / " << house_members.size() << " = $" << split_static_total << " per member" << std::endl << std::endl;
    
    reportFile << "2. Variable charges (gas + variable electric + variable water) divided among LIVING members only" << std::endl;
    reportFile << "   $" << total_variable << " / " << living_members.size() << " = $" << split_variable_total << " per living member" << std::endl << std::endl;
    
    reportFile << "FINAL SPLIT AMOUNTS" << std::endl;
    reportFile << "--------------------------------------------------" << std::endl;
    for (const auto& [member, amount] : split_map) {
      bool is_living = std::find(living_members.begin(), living_members.end(), member) != living_members.end();
      reportFile << std::left << std::setw(10) << member << ": $" << std::right << std::setw(8) << amount;
      if (is_living) {
        reportFile << " (Base: $" << std::setw(6) << split_static_total << " + Variable: $" << std::setw(6) << split_variable_total << ")";
      } else {
        reportFile << " (Base charges only)";
      }
      reportFile << std::endl;
    }
    
    reportFile << std::endl << "==================================================" << std::endl;
    reportFile << "Report generated: " << current_time_and_date;
    reportFile << "==================================================" << std::endl;
    
    reportFile.close();
    std::cout << std::endl << "Detailed report saved to: " << fullPath << std::endl;
  } else {
    std::cout << std::endl << "Error: Could not create report file at " << fullPath << std::endl;
  }
}

void view_previous_splits() {
  clear_screen();
  std::string reportsDir = std::string(getenv("HOME")) + "/.cache/rock_splits/reports";
  
  std::string mkdirCmd = "mkdir -p \"" + reportsDir + "\"";
  system(mkdirCmd.c_str());
  
  std::vector<std::string> reportFiles;
  
  DIR* dir = opendir(reportsDir.c_str());
  if (dir == nullptr) {
    std::cout << "Error: Could not open reports directory at " << reportsDir << std::endl;
    std::cout << "Press Enter to return to main menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    main_menu();
    return;
  }
  
  struct dirent* entry;
  while ((entry = readdir(dir)) != nullptr) {
    std::string filename = entry->d_name;
    if (filename != "." && filename != ".." && filename.find("rock_splits_report_") == 0) {
      reportFiles.push_back(filename);
    }
  }
  closedir(dir);
  std::sort(reportFiles.begin(), reportFiles.end(), std::greater<std::string>());
  
  std::cout << "Previous Reports\n";
  std::cout << "----------------\n\n";
  
  if (reportFiles.empty()) {
    std::cout << "No reports found.\n";
    std::cout << "Press Enter to return to main menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    main_menu();
    return;
  }
  
  for (size_t i = 0; i < reportFiles.size(); ++i) {
    std::string displayName = reportFiles[i].substr(18, reportFiles[i].length() - 22);
    std::replace(displayName.begin(), displayName.end(), '_', ' ');
    std::replace(displayName.begin(), displayName.end(), '-', ':');
    
    std::cout << i + 1 << ". " << displayName << std::endl;
  }
  
  std::cout << "\n" << reportFiles.size() + 1 << ". Return to main menu\n";
  std::cout << "\nSelect a report to view (1-" << reportFiles.size() + 1 << "): ";
  
  int choice;
  std::cin >> choice;
  std::cin.ignore();
  
  if (choice <= 0 || choice > static_cast<int>(reportFiles.size() + 1)) {
    std::cout << "Invalid choice. Please try again.\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
    view_previous_splits();
    return;
  }
  
  if (choice == static_cast<int>(reportFiles.size() + 1)) {
    main_menu();
    return;
  }
  
  std::string reportPath = reportsDir + "/" + reportFiles[choice - 1];
  std::ifstream reportFile(reportPath);
  
  if (reportFile.is_open()) {
    clear_screen();
    std::cout << "Report: " << reportFiles[choice - 1].substr(18, reportFiles[choice - 1].length() - 22) << "\n";
    std::cout << "----------------------------------------------------------------\n\n";
    
    std::string line;
    while (std::getline(reportFile, line)) {
      std::cout << line << std::endl;
    }
    
    reportFile.close();
  } else {
    std::cout << "Error: Could not open report file at " << reportPath << std::endl;
  }
  
  std::cout << "\nPress Enter to return to previous reports...";
  std::cin.get();
  view_previous_splits();
}

std::vector<float> split_comma_separated_string(const std::string& input){
  std::vector<float> result;
  std::istringstream iss(input);
  std::string token;
  while (std::getline(iss, token, ',')) {
    try {
      float value = std::stof(token);
      result.push_back(value);
    } catch (const std::invalid_argument& e) {
      std::cerr << "Warning: Invalid input '" << token << "'. Skipping. You may need to restart this report." << std::endl;
    }
  }
  return result;
}

std::string get_current_time_and_date() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  return std::ctime(&now_c);
}
