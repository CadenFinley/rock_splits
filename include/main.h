void clear_screen();
void main_menu();
void start_new_split();
void view_previous_splits();
void generate_report();
std::string get_current_time_and_date();
std::vector<float> split_comma_separated_string(const std::string& input);
void load_config();
void load_members();
void save_members();
void manage_members();

float gas_total = 0.0f;
// float gas_static = 0.0f;

float electric_total = 0.0f;
float electric_static = 0.0f;
std::vector<float> electric_static_charges;

float water_total = 0.0f;
float water_static = 0.0f;
std::vector<float> water_static_charges;

float internet_total = 0.0f;
// float internet_static = 0.0f;

std::vector<std::string> house_members;
std::map<std::string, float> split_map;

std::string current_time_and_date;
