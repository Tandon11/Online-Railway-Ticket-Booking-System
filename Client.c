#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdbool.h>
#define size 500
#define PORT 9001

int auto_id = 1000;

int main(){
        char buffer[size];
        char to_server[size];
        char username[size];
        char password[size];

        int i, q, p, ch, choice, socket_fd;

        struct sockaddr_in server_addr;
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);
        if(connect(socket_fd, (void *)(&server_addr), sizeof(server_addr)) <0 )
        {
                printf("Connection Failed....\n");
                return -1;
        }

        printf("\n-----------------------| Welcome To Online Railway Ticket Booking System |----------------------\n");

        printf("\n Enter Choice You want to login as \n  User --> 0 \n Agent --> 1 \n Admin --> 2\n");
        scanf("%d",&choice);
        choice = htonl(choice);
        write(socket_fd, &choice, sizeof(choice));

        printf("Enter Username : ");
        scanf("%s",username);
        write(socket_fd, &username, sizeof(username));

        read(socket_fd, &p, sizeof(p));

        if(p == 1)
        {
                printf("Enter Password : ");
                scanf("%s",password);

                write(socket_fd, &password, sizeof(password));
                read(socket_fd, &p, sizeof(p));

                if(p != 0)
                {
                        if(p == 10)
                        {
                                printf("\n----------------------| Welcome User %s |----------------------\n",username);
                                bool exit_status = false;

                                while(true)
                                {
                                        printf("Choose One Option :- \n");
                                        printf("1.) View Train Lists \n");
                                        printf("2.) View Bookings \n");
                                        printf("3.) Book Tickets \n");
                                        printf("4.) Update Bookings \n");
                                        printf("5.) Cancel Bookings \n");
                                        printf("6.) Exit \n");

                                        scanf("%d",&ch);

                                        printf("\n\n|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|\n\n");

                                        ch = htonl(ch);
                                                                write(socket_fd, &ch, sizeof(ch));			// write choice to server.
                                                
                                                                ch=ntohl(ch);
                                                                int train_num, type, status, temp, l, p, q;
                                                                
                                        switch(ch){
                                                
                                                case 1:
                                                        printf("--------Train Lists----------\n");
                                                        read(socket_fd, &temp, sizeof(temp));
                                                        printf("total trains = %d\n", temp);
                                                        l = temp;
                                                        for(i=0; i<l; i++)
                                                        {
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\ntrain_num - %d", temp);
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\nTotal_seats - %d", temp);
                                                                p = temp;
                                                                
                                                                read(socket_fd, &temp, sizeof(temp)); //booked_seats to be returned
                                                                printf("\navailable seats - %d", p-temp);

                                                                read(socket_fd, &temp, sizeof(temp)); 
                                                                
                                                                if(temp==1)
                                                                        printf("\nTrain is Active\n");
                                                                else
                                                                        printf("\nTrain is Cancelled\n");
                                                        }
                                                        
                                                        printf("\n---------------------------------------------------------------------\n");
                                                
                                                break;
                                                
                                                case 2:
                                                        printf("--------Booking Lists----------\n");
                                                        read(socket_fd, &temp, sizeof(temp));	
                                                        printf("total Booking = %d\n", temp);
                                                        l = temp;
                                                        
                                                        for(i=0; i<l; i++)
                                                        {
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\nBooking id - %d", temp);
                                                                
                                                                read(socket_fd, buffer , sizeof(buffer));
                                                                printf("\nuserName - %s", buffer);
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\ntrain_num - %d", temp);
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\nSeats Booked - %d\n", temp);
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                printf("\nStatus - %d (0 - cancel)\n", temp);
                                                        }
                                                        
                                                        printf("\n---------------------------------------------------------------------\n");
                                                        
                                                break;

                                                case 3:
                                                        printf("Enter train Num to book\n");
                                                        scanf("%d", &p);
                                                        p=htonl(p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        
                                                        printf("Enter no. of seats to book\n");
                                                        scanf("%d", &p);
                                                        p=htonl(p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        p = auto_id;
                                                        auto_id++;
                                                        p = htonl(p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        
                                                        read(socket_fd, &temp, sizeof(temp));
                                                        temp = ntohl(temp);
                                                        
                                                        if (temp == 1)
                                                                printf("Booked Successfully\n");
                                                        
                                                        else if(temp == 2)
                                                                printf("Train booking unSuccessful as Train is cancel\n");
                                                        
                                                        else	
                                                                printf("Booking Unsuccessful\n");
                                                        
                                                        printf("\n---------------------------------------------------------------------\n");
                                                
                                                break;
                                                
                                                case 4:
                                                        printf("Enter booking id to update\n");
                                                        scanf("%d", &p);
                                                        p=htonl(p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        
                                                        printf("Enter no. of seats to inc or dec\n"); 
                                                        scanf("%d", &p);
                                                        p=htonl(p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        
                                                        read(socket_fd, &temp, sizeof(temp));
                                                        temp = ntohl(temp);
                                                        
                                                        if (temp == 1)
                                                                printf("Booking updated Successfully\n");
                                                        
                                                        else	
                                                                printf("Booking update Unsuccessful\n");
                                                                
                                                        printf("---------------------------------------------------------------------");
                                                
                                                break;
                                                
                                                case 5:
                                                        printf("Enter booking id to cancel\n");
                                                        scanf("%d", &p);
                                                        write(socket_fd, &p, sizeof(p));
                                                        
                                                        printf("Deleted successfully\n");
                                                        
                                                        printf("\n---------------------------------------------------------------------\n");
                                                
                                                break;
                                                
                                                case 6:
                                                        printf("THANK YOU\n");
                                                        exit_status = true;
                                                break;
                                                                
                                                                }
                                        
                                        if(exit_status)
                                        break;
                                }
	 	
                       }

                       else if(p == 12){
                                printf("\n----------------------| Welcome Admin %s |----------------------\n",username);
                                bool exit_status = false;

                                while(true)
                                {
                                        printf("Choose One Option :- \n");
                                        printf("1.) View Train Lists \n");
                                        printf("2.) View Bookings \n");
                                        printf("3.) View Users \n");
                                        printf("4.) Delete Bookings \n");
                                        printf("5.) Add Users \n");
                                        printf("6.) Add Train \n");
                                        printf("7.) Delete User \n");
                                        printf("8.) Cancel Train \n");
                                        printf("9.) Resume Cancelled Train \n");
                                        printf("10.) Add Agent \n");
                                        printf("11.) Exit \n");

                                        scanf("%d",&ch);

                                        printf("\n\n|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|\n\n");

                                        ch = htonl(ch);

                                        write(socket_fd, &ch, sizeof(ch));			// write choice to server.
					
					ch = ntohl(ch);
					int train_num, type, status, temp, l, p, q;
					
                                        switch(ch){
						case 1:
							printf("--------Train Lists----------\n");
							read(socket_fd, &temp, sizeof(temp));
							
                                                        printf("total trains = %d\n", temp);
							l = temp;
							
                                                        for(i=0; i<l; i++)
                                                        {
								read(socket_fd, &temp, sizeof(temp));
								printf("\ntrain_num - %d", temp);
								
								read(socket_fd, &temp, sizeof(temp));
								printf("\nTotal_seats - %d", temp);
								p = temp;
								
								read(socket_fd, &temp, sizeof(temp)); //booked_seats to be returned
								printf("\navailable seats - %d", p-temp);

								read(socket_fd, &temp, sizeof(temp)); 
								
                                                                if(temp == 1)
									printf("\nTrain is Active\n");
								else
									printf("\nTrain is Cancelled\n");
							}

							printf("\n---------------------------------------------------------------------\n");
						break;
						
						case 2:
							printf("--------Booking Lists----------\n");
							read(socket_fd, &temp, sizeof(temp));	
							
                                                        printf("total Booking = %d\n", temp);
							l = temp;
							
                                                        for(i=0; i<l; i++)
                                                        {
								read(socket_fd, &temp, sizeof(temp));
								printf("\nBooking id - %d", temp);
								
								read(socket_fd, buffer , sizeof(buffer));
								printf("\nUserName - %s", buffer);
								
								read(socket_fd, &temp, sizeof(temp));
								printf("\ntrain_num - %d", temp);
								
								read(socket_fd, &temp, sizeof(temp));
								printf("\nSeats Booked - %d\n", temp);
								
								read(socket_fd, &temp, sizeof(temp));
								printf("\nStatus - %d (0 - cancle)\n", temp);
							}
							
							printf("\n---------------------------------------------------------------------\n");
						break;
							
						case 3:
							printf("--------Customers Lists----------\n");
							read(socket_fd, &temp, sizeof(temp));	
							printf("total Customers = %d\n", temp);
							l = temp;
							for(i=0; i<l; i++){
								read(socket_fd, buffer, sizeof(buffer));
								printf("\nusername - %s", buffer);
								
								read(socket_fd, buffer , sizeof(buffer));
								printf("\npassword - %s", buffer);
								
								read(socket_fd, &temp, sizeof(temp));
								temp = ntohl(temp);
								printf("\nType - %d", temp);
								
								read(socket_fd, &temp, sizeof(temp));
								temp = ntohl(temp);
								printf("\nStatus - %d", temp);
								
								printf("\nBooking ids - \n");
								for(p=0;p<10;p++)
                                                                {
									read(socket_fd, &temp, sizeof(temp));
									temp = ntohl(temp);
									
                                                                        if (temp!=0)
										printf("%d ", temp);
								}

								printf("\n");
								
							}
							
							printf("\n---------------------------------------------------------------------\n");
						break;
							
						case 4:
							printf("\nEnter booking id to cancel: ");
							scanf("%d", &p);
							p = htonl(p);
							write(socket_fd, &p, sizeof(p));
							
							printf("\nDeleted successfully\n");
							
							printf("\n---------------------------------------------------------------------\n");
						break;
						
						case 5:
							printf("\nEnter Username to be added: ");
							char input[size];
							scanf("%s",input);
							write(socket_fd, input, sizeof(input));

							printf("\nenter password of user to add: ");
							scanf("%s", password);
							write(socket_fd, password, sizeof(password));
							
							printf("\nenter user type: (should be zero for normal user): ");
							scanf("%d", &p); //should be 0 here
							p = htonl(p);
							write(socket_fd, &p, sizeof(p));
							
							read(socket_fd, &temp, sizeof(temp));		
							printf("value of temp %d",temp);

							if(temp == 0)
								printf("\nUsername already exists\n");
							else
								printf("\nuser added successfully\n");

							printf("\n---------------------------------------------------------------------\n");
						break;
						
						case 6:
							printf("Enter a unique train number: ");
							scanf("%d",&p);
							p = htonl(p);
							write(socket_fd, &p, sizeof(p));
							
							printf("Enter a total number of seats: ");
							scanf("%d",&p);
							p = htonl(p);
							write(socket_fd, &p, sizeof(p));
							
							read(socket_fd, &temp, sizeof(temp));		
							temp = ntohl(temp);
							printf("Temp Value : %d\n",temp);
                                                        if(temp == 0)
								printf("\nTrain added Successfully...\n");
							else
								printf("\nTrain Number Already Exists...\n");

							printf("\n---------------------------------------------------------------------\n");
						break;
						//7. Delete User \n8. Cancel train \n9. Resume Cancelled train \n10. EXIT\n");
						
                                                case 7:
							printf("Enter the user name to Delete : ");
							scanf("%s", input);
							write(socket_fd, input, sizeof(input));

							printf("Enter the type of user : ");
							scanf("%d",&p);
							write(socket_fd, &p, sizeof(p));

							read(socket_fd, &temp, sizeof(temp));		
							
                                                        if (temp == 1)
								printf("User deleted successfully\n");
							
                                                        else if (temp == 2)
								printf("User already deleted\n");
							
                                                        else	
								printf("Enter valid User name\n");

							printf("\n---------------------------------------------------------------------\n");
						break;

						case 8:
							printf("Enter the train Number to be cancled : ");
							scanf("%d",&p);
							write(socket_fd, &p, sizeof(p));

							read(socket_fd, &temp, sizeof(temp));		
							
                                                        if (temp == 1)
								printf("Train cancelled successfully\n");
							
                                                        else if (temp == 2)
								printf("Train already Cancelled\n");
							
                                                        else	
								printf("Enter valid train number\n");

							printf("\n---------------------------------------------------------------------\n");
						break;

						case 9:
							printf("Enter the train Number to be Resumed : ");
							scanf("%d",&p);
							write(socket_fd, &p, sizeof(p));

							read(socket_fd, &temp, sizeof(temp));		
							
                                                        if (temp == 1)
								printf("Train resumed successfully\n");
							
                                                        else if (temp == 2)
								printf("Train already in Running status\n");
							
                                                        else	
								printf("Enter valid train number\n");

							
							printf("\n---------------------------------------------------------------------\n");
						break;
						
						case 10:
							printf("\nEnter Username to be added: ");
							char inputU[size];
							scanf("%s",inputU);
							write(socket_fd, inputU, sizeof(input));
							
                                                        printf("\nenter password of user to add: ");
							scanf("%s", password);
							write(socket_fd, password, sizeof(password));
							
							printf("\nenter user type: (should be 1 for agent): ");
							scanf("%d", &p); //should be 1 here
							p = htonl(p);
							write(socket_fd, &p, sizeof(p));
							
							read(socket_fd, &temp, sizeof(temp));		
							printf("value of temp %d",temp);
							
                                                        if(temp == 0)
								printf("\nUsername already exists\n");
							else
								printf("\nAgent added successfully\n");

							printf("\n---------------------------------------------------------------------\n");
						break;

						case 11:
							printf("THANK YOU\n");
							exit_status = true;
						break;
					}

					if (exit_status)
						break;
	 		        }
	 	        }

                        else if(p == 11){
                                printf("\n----------------------| Welcome Agent %s |----------------------\n",username);
                                bool exit_status = false;

                                while(true)
                                {
                                        printf("Choose One Option :- \n");
                                        printf("1.) View Train Lists \n");
                                        printf("2.) View Bookings \n");
                                        printf("3.) Book Tickets \n");
                                        printf("4.) Update Bookings \n");
                                        printf("5.) Cancel Bookings \n");
                                        printf("6.) Exit \n");

                                        scanf("%d",&ch);

                                        printf("\n|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|-|*|\n");

                                        ch = htonl(ch);
                                        write(socket_fd, &ch, sizeof(ch));			// write choice to server.
                                                
                                        ch = ntohl(ch);

                                        int train_num, type, status, temp, l, p, q;
                                                switch(ch){
                                                        case 1:
                                                                printf("--------Train Lists----------\n");
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                
                                                                printf("total trains = %d\n", temp);
                                                                l = temp;
                                                                
                                                                for(i=0; i<l; i++)
                                                                {
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        printf("\ntrain_num - %d", temp);
                                                                        
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        printf("\nTotal_seats - %d", temp);
                                                                        p = temp;
                                                                        
                                                                        read(socket_fd, &temp, sizeof(temp)); //booked_seats to be returned
                                                                        printf("\navailable seats - %d", p-temp);

                                                                        read(socket_fd, &temp, sizeof(temp)); 
                                                                        
                                                                        if(temp == 1)
                                                                                printf("\nTrain is Active\n");
                                                                        else
                                                                                printf("\nTrain is Cancelled\n");
                                                                }
                                                                
                                                                printf("\n---------------------------------------------------------------------\n");
                                                        break;
                                                        
                                                        case 2:
                                                                printf("--------Booking Lists----------\n");
                                                                read(socket_fd, &temp, sizeof(temp));	
                                                                
                                                                printf("total Booking = %d\n", temp);
                                                                l = temp;
                                                                
                                                                for(i=0; i<l; i++)
                                                                {
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        printf("\nBooking id - %d", temp);
                                                                        
                                                                        read(socket_fd, buffer , sizeof(buffer));
                                                                        printf("\nuserName - %s", buffer);
                                                                        
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        printf("\ntrain_num - %d", temp);
                                                                        
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        temp = ntohl(temp);
                                                                        printf("\nSeats Booked - %d\n", temp);
                                                                        
                                                                        read(socket_fd, &temp, sizeof(temp));
                                                                        temp = htonl(temp);
                                                                        printf("\nStatus - %d (0 - cancle)\n", temp);
                                                                }
                                                                
                                                                printf("\n---------------------------------------------------------------------\n");
                                                        break;
                                                        
                                                        case 3:
                                                                printf("Enter train Num to book\n");
                                                                scanf("%d", &p);
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                printf("Enter no. of seats to book\n");
                                                                scanf("%d", &p);
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                p = auto_id;
                                                                auto_id++;
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                temp = ntohl(temp);
                                                                
                                                                if (temp == 1)
                                                                        printf("Booked Successfully\n");
                                                                else if(temp == 2)
                                                                        printf("Train booking unSuccessful as Train is cancel\n");
                                                                else	
                                                                        printf("Booking Unsuccessful\n");
                                                                
                                                                printf("\n---------------------------------------------------------------------\n");
                                                        break;
                                                        
                                                        case 4:
                                                                printf("Enter booking id to update\n");
                                                                scanf("%d", &p);
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                printf("Enter no. of seats to inc or dec\n"); 
                                                                scanf("%d", &p);
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                read(socket_fd, &temp, sizeof(temp));
                                                                temp = ntohl(temp);
                                                                
                                                                if (temp == 1)
                                                                        printf("Booking updated Successfully\n");
                                                                else	
                                                                        printf("Booking update Unsuccessful\n");
                                                                        
                                                                printf("\n---------------------------------------------------------------------\n");
                                                        break;
                                                        
                                                        case 5:
                                                                printf("Enter booking id to cancel\n");
                                                                scanf("%d", &p);
                                                                p = htonl(p);
                                                                write(socket_fd, &p, sizeof(p));
                                                                
                                                                printf("Deleted successfully\n");
                                                                
                                                                printf("\n---------------------------------------------------------------------\n");
                                                        break;
                                                        
                                                        case 6:
                                                                printf("THANK YOU\n");
                                                                exit_status = true;
                                                        break;
                                                                                
                                                }
                                                printf("\n");
                                                if (exit_status)
                                                        break;
                                }

			}


	        }

	 	else
                {
			 printf("wrong password\n");
		}
        }

        else if (p == 2)
                printf("User Already Logged in Please Logout and Try Again !!!\n");

        else
                printf("Wrong Username Please Enter Correct one\n");

        return 0;
}