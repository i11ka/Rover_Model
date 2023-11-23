#include "Boundary.h"

Boundary::Boundary(const string & config_filename){
    if(read_config(config_filename) == 0){
        cout << "\nIncorrect config\n";
    }
    // cout << boundary_log_filename << " " << rover_command_filename << " " << surface_command_filename << " " << controller_log_filename << " " << gnu_surf_filename << " " << gnu_rover_filename; 
    boundary_log_file.open(boundary_log_filename);
    controller = new Control(is_date, controller_log_filename, gnu_surf_filename, gnu_rover_filename, gnu_load_filename);
    read_surface_config();
    read_rover_config();
    controller->close_files();
    // delete controller;
    // controller->surface_delete();
    // free(controller);  
}

bool Boundary::read_config(const string & conf_name)
{
    string buffer;
    bool is_surf_command = false, is_rover_command = false, is_cntrl_log = false, is_bndry_log = false;
    config_file.open(conf_name);
    while(getline(config_file, buffer)){
        // cout << buffer << " \n";
         if(is_substr(buffer, "print date")){
            if(is_substr(buffer, "YES")){
                is_date = true;
            }
            else if(is_substr(buffer, "NO")){
                is_date = false;
            }
            else{
                cout << "Error! Wrong command to set date in log file! Check how it spells in sample config file! \n";
                return 0;
            }
        }
        else if(is_substr(buffer,"Surface command file")){
            is_surf_command = fill_filename(buffer, surface_command_filename);
        }
        else if(is_substr(buffer,"Rover command file")){
            is_rover_command = fill_filename(buffer, rover_command_filename);
        }
        else if(is_substr(buffer,"Log boundary file")){
            is_bndry_log = fill_filename(buffer, boundary_log_filename);
        }
        else if(is_substr(buffer,"Log control file")){
            is_cntrl_log = fill_filename(buffer, controller_log_filename);
        }
        else if(is_substr(buffer,"GNU file for surface")){
            fill_filename(buffer, gnu_surf_filename);
        }
        else if(is_substr(buffer,"GNU file for rover")){
            fill_filename(buffer, gnu_rover_filename);
        }
        else if(is_substr(buffer,"The final GNU file")){
            fill_filename(buffer, gnu_load_filename);
        }
        else{
            cout << "\n Incorrect command in config file. Compare with sample! \n";
        }
    }
    if(is_surf_command && is_rover_command && is_bndry_log && is_cntrl_log)
            return true;
        else return false;
    return false;
}

bool Boundary::read_surface_config(){
    string buffer;
    ifstream surface_command_file;
    surface_command_file.open(surface_command_filename);
    bool is_surface = false;
    int is_rand = 0;
    while(getline(surface_command_file, buffer)){
        // cout << buffer << "\n";
        if(is_substr(buffer, "Random hummocks")){
            double rand_humm_number;
            if(sscanf(buffer.c_str(), "%*[^=]=%lf;", &rand_humm_number) == 1 && !is_extra_symbol_after_semicolon(buffer)){
                print_message_in_log("Command to define number of random hummocks has been read! ");
                if(controller->surface_set_random_hummocks(rand_humm_number)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
                is_rand += 1;
            }
            else{
                print_message_in_log("ERROR! Wrong command to set number of random hummocks. You need to write 1 positive integer number as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_substr(buffer, "Random stones")){
            double temp;
            if(sscanf(buffer.c_str(), "%*[^=]=%lf;", &temp) == 1 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to define number of random stones has been read! ");
                if(controller->surface_set_random_stones(temp)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
                is_rand += 1;
            }
            else{
                print_message_in_log("ERROR! Wrong command to set number of random stones. You need to write 1 number as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_substr(buffer, "Random logs")){
            double temp;
            if(sscanf(buffer.c_str(), "%*[^=]=%lf;", &temp) == 1 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to define number of random logs has been read! ");
                if(controller->surface_set_random_logs(temp)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
                is_rand += 1;
            }
            else{
                print_message_in_log("ERROR! Wrong command to set number of random logs. You need to write 1 number as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_rand == 3 && is_substr(buffer, "Create surface A: ")){
            double temp1, temp2, temp3;

            if(sscanf(buffer.c_str(), "%*[^=]=%lf,%*[^=]=%lf,%*[^=]=%lf;", &temp1, &temp2, &temp3) == 3 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to create surface has been read! ");
                if(controller->surface_create(temp1, temp2, temp3)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
                is_surface = true;
            }
            else{
                print_message_in_log("ERROR! Wrong command to create surface. You need to write 3 numbers as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }   
        }
        else if(is_surface && is_substr(buffer,"Set cursor")){
            double temp1, temp2;
            if(sscanf(buffer.c_str(), "%*[^(](%lf, %lf)", &temp1, &temp2) == 2 && is_extra_symbol_after_semicolon(buffer) == 0){ 
                print_message_in_log("Command to define cursor location has been read!");
                if(controller->set_cursor(temp1, temp2)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
            }    
            else{
                print_message_in_log("ERROR! Wrong command to set cursor location. You need to write 2 numbers in parentheses separated by commas as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_surface && is_substr(buffer,"Create hummock(")){
            double temp1, temp2, temp3, temp4;
            if(sscanf(buffer.c_str(), "%*[^(](%lf, %lf, %lf, %lf);", &temp1, &temp2, &temp3, &temp4) == 4 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to create hummock has been read! ");
                if(controller->surface_create_hummock(temp1, temp2, temp3, temp4)== 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
            }
            else{
                print_message_in_log("ERROR! Wrong command to create hummock. You need to write 4 numbers in parentheses separated by commas as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_surface && is_substr(buffer,"Create stone")){
            double temp;
            if(sscanf(buffer.c_str(), "%*[^(](%lf);", &temp) == 1 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to create stone has been read! ");
                if(controller->surface_create_stone(temp) == 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
            }
            else{
                print_message_in_log("ERROR! Wrong command to create stone. You need to write 1 number in parentheses as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_surface && is_substr(buffer,"Create log")){
            double temp1, temp2, temp3, temp4, temp5;
            if(sscanf(buffer.c_str(), "%*[^(](%lf, %lf, %lf, %lf, %lf);", &temp1, &temp2, &temp3, &temp4, &temp5) == 5 && is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to create log has been read! ");
                if(controller->surface_create_log(temp1, temp2, temp3, temp4, temp5) == 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
            }
            else{
                print_message_in_log("ERROR! Wrong command to create log. You need to write 5 numbers in parentheses separated by commas as described in config sample and don`t write something after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else if(is_surface && is_substr(buffer,"Count surface;")){
            if(is_extra_symbol_after_semicolon(buffer) == 0){
                print_message_in_log("Command to count surface has been read! ");
                if(controller->surface_count() == 0){
                    print_message_in_log("The Controller did not accept these values ");
                    surface_command_file.close();
                    return 0;
                }
                is_counted = true;
            }
            else{
                print_message_in_log("ERROR! Wrong command to count surface! Try delete extra symbols after semicolon! ");
                surface_command_file.close();
                return 0;
            }
        }
        else{
            print_message_in_log("ERROR! UNKNOWN COMMAND! ");
            surface_command_file.close();
            return 0;
        }
    }
    surface_command_file.close();
    return 1; 
}

void Boundary::print_message_in_log(const string & message)
{
    if(is_date){
        printer_with_time(message, boundary_log_file);
    }
    else{
        boundary_log_file << message <<"\n";
    }
}

bool Boundary::fill_filename(const string & line, string & filename)
{
    int i = 0;
    while(line[i] != '"'){
        i++;
    }
    i++;
    for (int j = i; j < 128 && line[j] != '"'; j++)
    {
        filename += line[j];
    }
    if(filename.empty()) return false;
    else return true;
}

bool Boundary::read_rover_config(){
    bool is_rover = false;
    string buffer; 
    rover_command_file.open(rover_command_filename);
    while (getline(rover_command_file, buffer))
    {
        if(is_substr(buffer, "Create Rover")){
            // int temp1, temp2, temp3, temp4, temp9, temp10;
            // double temp5, temp6, temp7, temp8;
            int temp1, temp2;
            double temp5, temp6, temp7, temp8;
            // if(sscanf(buffer.c_str(), "%*[^(](%d, %d, %d, %d, %lf, %lf, %lf, %lf, %d, %d);", &temp1, &temp2, &temp3, &temp4, &temp5, &temp6, &temp7, &temp8, &temp9, &temp10) !=0 && is_extra_symbol_after_semicolon(buffer) == 0){
            //     // cout << temp1 << temp2 << temp3 << temp4 << temp5 << temp6 << temp7 << temp8 << temp9 << temp10;
            //     print_message_in_log("Command to create rover has been read! ");
            //     controller->rover_create(temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9, temp10);
            //     is_rover = true;
            // }
            if(sscanf(buffer.c_str(), "%*[^(](%d, %d, %lf, %lf, %lf, %lf);", &temp1, &temp2, &temp5, &temp6, &temp7, &temp8) !=0 && is_extra_symbol_after_semicolon(buffer) == 0){
                // cout << temp1 << temp2 << temp3 << temp4 << temp5 << temp6 << temp7 << temp8 << temp9 << temp10;
                print_message_in_log("Command to create rover has been read! ");
                controller->rover_create(temp1, temp2, temp5, temp6, temp7, temp8);
                is_rover = true;
            }
            else{
                print_message_in_log("ERROR! Wrong command to create rover. You need to write 10 numbers as described in config sample and don`t write something after semicolon! ");
                rover_command_file.close();
                return 0;
            }
        }
        else if(is_rover && is_substr(buffer, "turn 180;")){
            print_message_in_log("Command to turn rover has been read! ");
            controller->rover_turn_180();
        }
        else if(is_rover && is_substr(buffer, "turn 90 clockwise;")){
            print_message_in_log("Comand to turn rover has been read! ");
            if(controller->rover_turn_90_clock() == 0){
                print_message_in_log("The rover broke down ");
                rover_command_file.close();
                return 0;
            }
        }
        else if(is_rover && is_substr(buffer, "turn 45 clockwise;")){
            print_message_in_log("Comand to turn rover has been read! ");
            if(controller->rover_turn_45_clock() == 0){
                print_message_in_log("The rover broke down ");
                rover_command_file.close();
                return 0;
            }
        }
        else if(is_rover && is_substr(buffer, "turn 90 counterclockwise;")){
            print_message_in_log("Comand to turn rover has been read! ");
            if(controller->rover_turn_90_counterclock() == 0){
                print_message_in_log("The rover broke down ");
                rover_command_file.close();
                return 0;
            }
        }
        else if(is_rover && is_substr(buffer, "turn 45 counterclockwise;")){
            print_message_in_log("Comand to turn rover has been read! ");
            if(controller->rover_turn_45_counterclock() == 0){
                print_message_in_log("The rover broke down ");
                rover_command_file.close();
                return 0;
            }
        }
        else if(is_rover && is_substr(buffer, "drive forward at a speed of 1;")){
            print_message_in_log("Comand to move the rover has been read! ");
            if(controller->rover_drive_forward_1() == 0){
                print_message_in_log("The rover broke down ");
                rover_command_file.close();
                return 0;
            }
        }
        // else if(is_rover && is_substr(buffer, "drive forward at a speed of 2;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_forward_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive forward at a speed of 2;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_forward_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the northeast;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NE_1() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the northwest;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NW_1() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the NNE;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NNE_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the NEE;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NEE_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the NNW;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NNW_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        // else if(is_rover && is_substr(buffer, "drive to the NWW;")){
        //     print_message_in_log("Comand to move the rover has been read! ");
        //     if(controller->rover_drive_NWW_2() == 0){
        //         print_message_in_log("The rover broke down ");
        //         rover_command_file.close();
        //         return 0;
        //     }
        // }
        else if(is_rover && is_substr(buffer, "Create Rover")){
            print_message_in_log("ERROR! Wrong command. You can constructe only one rover! ");
            rover_command_file.close();
            return 0;
        }
        else{
            print_message_in_log("ERROR! UNKNOWN COMMAND! ");
        }
    }
    
   
    return 1;
    
}