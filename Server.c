#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include "Client_Utilities.h"
#include <stdbool.h>
#define PORT 9001
#define SIZE 500


int main(){
        remove("Customer.txt");

        if(check_Admin() == -1)
        {
                struct Customer cust;
                
                printf("Create Admin\nEnter Username : ");
                scanf("%s", cust.username);

                printf("Enter Password : ");
                scanf("%s", cust.password);

                cust.cust_type = 2;
                cust.cust_status = 1;

                for(int i=0; i<10; i++)
                        cust.booking_id[i] = 0;

                add_admin(cust);
        }

        struct Customer cust1;

        struct sockaddr_in server;
        struct sockaddr_in client;

        int socket_fd, client_fd;
        int temp, acno;
        int client_size = sizeof(client);

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PORT);

        char username[SIZE];
        char password[SIZE];
        char sample[SIZE];
        char sample2[SIZE];

        if(bind(socket_fd, (void *)(&server), sizeof(server)) < 0)
        {
                printf("Error in Binding...\n");
                exit(0);
        }

        if(listen(socket_fd, 5) < 0)
        {
                printf("Error in Listening...\n");
                exit(0);
        }

        while(true)
        {
                if((client_fd = accept(socket_fd, (struct sockaddr *)&client, &client_size)) < 0)
                {
                        printf("Error in Accepting...\n");
                        exit(0);
                }

                if(!fork())    // Child Process
                {
                        while(true)
                        {
                                int choice;
                                int buffer;

                                read(client_fd, &choice, sizeof(choice));
                                choice = ntohl(choice);
                                
                                read(client_fd, &username, sizeof(username));
                                
                                printf("Username : %s\n",username);
                                printf("Choice : %d\n",choice);

                                if(choice == 0 || choice == 2)
                                {
                                        buffer = is_user_valid(username, choice);
                                        printf("Value of Buffer%d\n",buffer);
                                }

                                else
                                {
                                        buffer = is_user_agent_valid(username);
                                }

                                write(client_fd, &buffer, sizeof(buffer));    // Sending Client whether user is valid or not

                                read(client_fd, &password, sizeof(password));    // Reading Password

                                if(choice == 0 || choice == 2)
                                {
                                        buffer = is_password_valid(username, password, choice);
                                }

                                else
                                {
                                        buffer = is_agent_password_valid(username, password);
                                }

                                write(client_fd, &buffer, sizeof(buffer));   // Sending client whether password is valid or not

                                if(buffer == 10)  // Normal User
                                {
                                            update_customer_status(username, choice, 2);    // 2 indicates user has login session active
                                            bool user_logged_out = false;

                                            while(true)
                                            {
                                                        int switchchoice;
                                                        read(client_fd, &switchchoice, sizeof(switchchoice));	// read choice inside admin
						        switchchoice = ntohl(switchchoice);

                                                        int b, train_no, booking_id, temp, len, q, seat;

                                                           switch(switchchoice){
                                                            case 1:
                                                            ;
                                                                    struct Train trn_arr[20];
                                                                    len = get_train_details(trn_arr);
                                                                    write(client_fd, &len, sizeof(len));

                                                                    for(q=0; q<len; q++)
                                                                    {
                                                                            temp = trn_arr[q].train_no;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            temp = trn_arr[q].total_seats;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            temp = trn_arr[q].booked_seats;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            temp = trn_arr[q].train_status;
                                                                            write(client_fd, &temp, sizeof(temp));
                                                                    }
                                                            break;
                                                            
                                                            case 2:
                                                            ;
                                                                    struct Booking book_arr[20];
                                                                    len = get_booking_details_by_username(book_arr, username);
                                                                    write(client_fd, &len, sizeof(len));

                                                                    for(q=0; q<len; q++)
                                                                    {
                                                                            temp = book_arr[q].booking_id;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            strcpy(sample, book_arr[q].username);
									    write(client_fd, &sample, sizeof(sample));

                                                                            temp = book_arr[q].train_no;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            temp = book_arr[q].seats_booked;
                                                                            write(client_fd, &temp, sizeof(temp));

                                                                            temp = book_arr[q].booking_status;
                                                                            write(client_fd, &temp, sizeof(temp));
                                                                    }
                                                            break;

                                                            case 3:
                                                                    read(client_fd, &train_no, sizeof(train_no));
                                                                    train_no = ntohl(train_no);

                                                                    read(client_fd, &seat, sizeof(seat));
                                                                    seat = ntohl(seat);

                                                                    read(client_fd, &booking_id, sizeof(booking_id));
                                                                    booking_id = ntohl(booking_id);

                                                                    struct Booking new_booking;
                                                                    new_booking.train_no = train_no;
                                                                    new_booking.booking_id = booking_id;
                                                                    new_booking.seats_booked = seat;
                                                                    strcpy(new_booking.username, username);
                                                                    new_booking.booking_status = 1;

                                                                    temp = add_booking(new_booking, username);
                                                                    temp = htonl(temp);

                                                                    write(client_fd, &temp, sizeof(temp));

                                                            break;

                                                            case 4:
                                                                    read(client_fd, &booking_id, sizeof(booking_id));
                                                                    booking_id = ntohl(booking_id);

                                                                    read(client_fd, &seat, sizeof(seat));
                                                                    seat = ntohl(seat);

                                                                    temp = update_booking(booking_id, seat, username);
                                                                    temp = htonl(temp);
                                                                    write(client_fd, &temp, sizeof(temp));

                                                            break;

                                                            case 5:
                                                                    read(client_fd, &booking_id, sizeof(booking_id));
                                                                    delete_booking_by_user(booking_id, username);
                                                            
                                                            break;

                                                            case 6:
                                                                    user_logged_out = true;
                                                                    update_customer_status(username, choice, 1); //1 indicates user is logged out
                                                                   
                                                            break;
                                                    

                                            }

                                        if(user_logged_out)
                                            break;
                                        }
                                }

                                else if(buffer == 12)   // Admin
                                {
                                                update_customer_status(username, choice, 2); //2 indicates admin is in login
                                                bool user_logged_out = false;
                                                
                                                while(true){
                                                        int switchChoice;
                                                        read(client_fd, &switchChoice, sizeof(switchChoice));	// read choice inside admin
                                                        switchChoice = ntohl(switchChoice);
                                                        
                                                        int b, train_no, booking_id, temp, len, q, seat;

                                                        switch(switchChoice){
                                                                case 1:
                                                                ;
                                                                        struct Train trn_arr[20];
                                                                        len = get_train_details(trn_arr);
                                                                        write(client_fd, &len, sizeof(len));
                                                                        
                                                                        for(q=0;q<len;q++)
                                                                        {
                                                                                temp = trn_arr[q].train_no;
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = trn_arr[q].total_seats;
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = trn_arr[q].booked_seats; 
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = trn_arr[q].train_status; 
                                                                                write(client_fd, &temp, sizeof(temp));
                                                                        }
                                                                break;

                                                                case 2:
                                                                ;
                                                                        struct Booking book_arr[20];
                                                                        len = get_booking_details(book_arr);
                                                                        write(client_fd, &len, sizeof(len));
                                                                        
                                                                        for(q=0;q<len;q++)
                                                                        {
                                                                                temp = book_arr[q].booking_id;
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                strcpy(sample, book_arr[q].username);
                                                                                write(client_fd, &sample, sizeof(sample));

                                                                                temp = book_arr[q].train_no;
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = book_arr[q].seats_booked;
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = book_arr[q].booking_status;
                                                                                write(client_fd, &temp, sizeof(temp));
                                                                        }
                                                                break;

                                                                case 3:
                                                                ;
                                                                        struct Customer cust_arr[20];
                                                                        len = get_user_details(cust_arr);
                                                                        write(client_fd, &len, sizeof(len));
                                                                        
                                                                        for(q=0;q<len;q++)
                                                                        {
                                                                                strcpy(sample, cust_arr[q].username);
                                                                                write(client_fd, &sample, sizeof(sample));

                                                                                strcpy(sample, cust_arr[q].password);
                                                                                write(client_fd, &sample, sizeof(sample));

                                                                                temp = cust_arr[q].cust_type;
                                                                                temp = htonl(temp);
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                temp = cust_arr[q].cust_status;
                                                                                temp = htonl(temp);
                                                                                write(client_fd, &temp, sizeof(temp));

                                                                                for(b=0; b<10; b++)
                                                                                {
                                                                                        temp = cust_arr[q].booking_id[b];
                                                                                        temp = htonl(temp);
                                                                                        write(client_fd, &temp, sizeof(temp));
                                                                                }

                                                                        }
                                                                break;
                                                                
                                                                case 4:
                                                                        read(client_fd, &booking_id, sizeof(booking_id));
                                                                        booking_id = ntohl(booking_id);

                                                                        delete_booking(booking_id);
                                                                break;
                                                                
                                                                case 5:
                                                                ;
                                                                        struct Customer cust;
                                                                        read(client_fd, cust.username, sizeof(cust.username));

                                                                        read(client_fd, cust.password, sizeof(cust.password));

                                                                        read(client_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        cust.cust_type = temp;

                                                                        cust.cust_status = 1;

                                                                        for(b=0;b<10;b++)
                                                                                cust.booking_id[b]  = 0;
                                                                        
                                                                        temp = add_user(cust);

                                                                        printf("temp is %d\n",temp);
                                                                        write(client_fd, &temp, sizeof(temp));

                                                                break;

                                                                case 6:
                                                                ;
                                                                        struct Train t;
                                                                        read(client_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        t.train_no = temp;

                                                                        read(client_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        t.total_seats = temp;

                                                                        t.booked_seats = 0;
                                                                        t.train_status = 1;
                                                                        
                                                                        temp = add_train(t);
                                                                        temp = htonl(temp);
                                                                        write(client_fd, &temp, sizeof(temp));
                                                                break;
                                                                
                                                                 //7. Delete User \n8. Cancel train \n9. Resume Cancelled train \n10. EXIT\n");
                                                                case 7:
                                                                        read(client_fd, &sample, sizeof(sample));
                                                                        read(client_fd, &temp, sizeof(temp));

                                                                        temp = delete_customer(sample, temp);
                                                                        write(client_fd, &temp, sizeof(temp));
                                                                break;

                                                                case 8:
                                                                        read(client_fd, &train_no, sizeof(train_no));

                                                                        temp = delete_train(train_no);
                                                                        write(client_fd, &temp, sizeof(temp));
                                                                break;
                                                                
                                                                case 9:
                                                                        read(client_fd, &train_no, sizeof(train_no));

                                                                        temp = resume_train(train_no);
                                                                        write(client_fd, &temp, sizeof(temp));
                                                                break;
                                                                
                                                                case 10:
                                                                ;
                                                                        struct Agent a;
                                                                        read(client_fd, a.username, sizeof(a.username));

                                                                        read(client_fd, a.password, sizeof(a.password));

                                                                        read(client_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        a.agent_type = temp;

                                                                        a.agent_status = 1;

                                                                        for(b=0;b<10;b++)
                                                                                a.booking_id[b]  = 0;
                                                                        
                                                                        temp = add_agent(a);
                                                                        printf("temp is %d\n",temp);
                                                                        write(client_fd, &temp, sizeof(temp));

                                                                break;

                                                                case 11:
                                                                        user_logged_out = true;
                                                                        update_customer_status(username, choice, 1); //1 indicates user is logged out
                                                                break;

                                                        }
                                                        if(user_logged_out)
                                                                break;
					}

				}

                                else if (buffer == 11)   // Agent
                                {
					bool exit_status = false;
					while(true)
                                        {
						int switchChoice;
						read(client_fd, &switchChoice, sizeof(switchChoice));	// read choice inside admin
						switchChoice = ntohl(switchChoice);
						
						int b, train_no, booking_id, temp, len, q, seat;
						
						switch(switchChoice){
							case 1:
							;
								struct Train trn_arr[20];
								len = get_train_details(trn_arr);
								write(client_fd, &len, sizeof(len));
								
								for(q=0;q<len;q++)
                                                                {
									temp = trn_arr[q].train_no;
									write(client_fd, &temp, sizeof(temp));

									temp = trn_arr[q].total_seats;
									write(client_fd, &temp, sizeof(temp));

									temp = trn_arr[q].booked_seats; 
									write(client_fd, &temp, sizeof(temp));

									temp = trn_arr[q].train_status; 
									write(client_fd, &temp, sizeof(temp));
								}
							break;
							
							case 2:
							;
								struct Booking book_arr[20];
								len = get_booking_details_by_username(book_arr, username);
								write(client_fd, &len, sizeof(len));
								
								for(q=0;q<len;q++)
                                                                {
									temp = book_arr[q].booking_id;
									temp = htonl(temp);
									write(client_fd, &temp, sizeof(temp));

									strcpy(sample, book_arr[q].username);
									write(client_fd, &sample, sizeof(sample));

									temp = book_arr[q].train_no;
									temp = htonl(temp);
									write(client_fd, &temp, sizeof(temp));

									temp = book_arr[q].seats_booked;
									temp = htonl(temp);
									write(client_fd, &temp, sizeof(temp));

									temp = book_arr[q].booking_status;
									temp = htonl(temp);
									write(client_fd, &temp, sizeof(temp));
								}
							break;

							case 3:
								read(client_fd, &train_no, sizeof(train_no));
								train_no = ntohl(train_no);

								read(client_fd, &seat, sizeof(seat));
								seat = ntohl(seat);

								read(client_fd, &booking_id, sizeof(booking_id));
								booking_id = ntohl(booking_id);

								struct Booking new_booking;
								new_booking.train_no = train_no;
								new_booking.booking_id = booking_id;
								new_booking.seats_booked = seat;
								strcpy(new_booking.username, username);
								new_booking.booking_status = 1;

								temp = add_booking_agent(new_booking, username);
								temp = htonl(temp);
								write(client_fd, &temp, sizeof(temp));

							break; 

							case 4:
								read(client_fd, &booking_id, sizeof(booking_id));
								booking_id = ntohl(booking_id);

								read(client_fd, &seat, sizeof(seat));
								seat = ntohl(seat);

								temp = update_booking(booking_id, seat, username);
								temp = htonl(temp);
								write(client_fd, &temp, sizeof(temp));

							break;

							case 5:
								read(client_fd, &booking_id, sizeof(booking_id));
								booking_id = ntohl(booking_id);

								delete_booking_by_user(booking_id, username);
							break;

							case 6:
								exit_status = true;
							break;
						}

						if(exit_status)
							break;
					}

                                }

                        }
                }
        }

        return 0;
}