#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define size 500


struct Customer{
            char username[size];
            char password[size];
            int booking_id[10];
            int cust_type;     // 0 -> Normal User   1-> Agent    2-> Admin
            int cust_status;   // 0 -> Customer deleted  1 -> Active customer  2-> Customer logged in  
};


struct Train{
            int train_no;
            int train_status;    // 0-> Train cancel    1-> Train Active
            int booked_seats;
            int total_seats;
};


struct Booking{
            char username[size];
            int booking_id;
            int train_no;
            int seats_booked;
            int booking_status;   // 0 -> Booking Cancel    1-> Booking available
};


struct Agent{
            char username[size];
	    char password[size];
	    int booking_id[10];
            int agent_type;
	    int agent_status;  // 0->  Agent Deleted  1->  Agent active  2->  Agent Logged in
};


long int check_Admin(){
            struct stat st;
            char *file_name = "Customer.txt";

            if(stat(file_name, &st) == 0)
            {
                    return st.st_size;
            }

            else
            {
                    return -1;
            }
}


int add_admin(struct Customer admin){
            int customer_fd = open("Customer.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
            write(customer_fd, &admin, sizeof(admin));
            close(customer_fd);
}


int is_user_valid(char* name, int choice){
            int customer_fd = open("Customer.txt", O_RDONLY, 0766);
            struct Customer cust;

            while(read(customer_fd, &cust, sizeof(cust)) > 0)
            {
                    if((cust.cust_type == choice) && (strcmp(cust.username, name) == 0) && cust.cust_status == 1)
                    {
                            return 1;
                    }

                    else if((cust.cust_type == choice) && (strcmp(cust.username, name) == 0) && cust.cust_status == 2)
                    {
                            return 2;
                    }
            }

            close(customer_fd);
            return 0;
}


int is_password_valid(char *name, char *password, int choice){
            int customer_fd = open("Customer.txt", O_RDONLY, 0766);
            int value;

            struct Customer cust;

            while(read(customer_fd, &cust, sizeof(cust)) > 0)
            {
                    if((cust.cust_type == choice) && (strcmp(cust.username, name) == 0) && (strcmp(cust.password, password) == 0))
                    {
                            value = cust.cust_type + 10;
                            return value;
                    }
            }

            close(customer_fd);
            return 0;
}


int add_train(struct Train trn){
            int train_fd = open("Train.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
            struct Train t;    // read value is stored here

            int train_exists = false;

            while(read(train_fd, &t, sizeof(t)) > 0)
            {
                    if(t.train_no - trn.train_no == 0)
                    {
                            train_exists = true;
                    }
            }

            if(!train_exists)    // train doesn't exists  
            {
                    struct flock lock;
                    lock.l_type = F_WRLCK;
                    lock.l_whence = SEEK_CUR;
                    lock.l_start = 0;
                    lock.l_len = 0;
                    lock.l_pid = getpid();

                    fcntl(train_fd, F_SETLKW, &lock);      // Entering Critical Section
                    write(train_fd, &trn, sizeof(trn));    // Writing new train to file

                    lock.l_type = F_UNLCK;
                    fcntl(train_fd, F_SETLKW, &lock);

                    close(train_fd);
                    return 0;
            }

            else
            {
                    return 1;
            }

        return -1;
}


int add_user(struct Customer cstmr){
	int customer_fd = open("Customer.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	struct Customer c1;
	
	int user_exists = false;
	while(read(customer_fd, &c1, sizeof(c1))>0)
	{
		if(strcmp(c1.username, cstmr.username) == 0 && c1.cust_status == 1)
		{
			user_exists = true;
			break;
		}
	}
	if(!user_exists)		// username doesnt exist
	{
		printf("\nunder not flag\n");
		struct flock lock;
		lock.l_type = F_WRLCK;	
		lock.l_whence = SEEK_CUR;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();

		fcntl(customer_fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
		write(customer_fd, &cstmr, sizeof(cstmr));
		
                lock.l_type = F_UNLCK;			// unlocking.
		fcntl(customer_fd, F_SETLK, &lock);		// unlocking.
		
		close(customer_fd);
		
		// -----------locking and writing into customer file-------------------
		
		return 1;
	}
	
        else
        {		// username exists.
		close(customer_fd);
		return 0;
        }
	
        close(customer_fd);
}


int get_user_details(struct Customer *arr)
{
	int customer_fd = open("Customer.txt", O_RDONLY, 0766);
	
	printf("User Details\n");
	
	struct Customer c;
	int i=0;
	
	while(read(customer_fd, &c, sizeof(c))>0)
	{
		if(c.cust_status == 1)
			printf("\n%s\t%s\t%d", c.username, c.password, c.cust_type);
			
		arr[i] = c;
		i++;
	}

	close(customer_fd);
	return i;
}



int get_train_details(struct Train *trn_arr){
                int train_fd = open("Train.txt", O_RDONLY, 0766);
                printf("Train Details\n");

                struct Train t;
                int i=0;
                
                while(read(train_fd, &t, sizeof(t))>0)
                {
                        if(t.train_status == 1)		
                                printf("\n%d\t%d\t%d", t.train_no, t.total_seats, t.booked_seats);
                        
                        trn_arr[i] = t;
                        i++;
                }

                close(train_fd);
                return i;
}


int add_booking(struct Booking bking, char *username){
                if (bking.seats_booked<=0)
                        return 0;
                int booking_fd = open("Booking.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
                
                /******counting total number of bookings*************/
                int count = 0;
                struct Booking b1;

                while(read(booking_fd, &b1, sizeof(b1)) > 0)
                        count++;

                bking.booking_id = 1000 + count + 1;
                printf("Booking Id - %d\n",bking.booking_id);
                close(booking_fd);

                /****User details to be checked and modified*************/
                int customer_fd = open("Customer.txt", O_RDWR, 0766); //for user i.e customer
                struct Customer c1;
                struct Customer c;
                
                int i=1;
                int x, y;
                
                while(read(customer_fd, &c1, sizeof(c1))>0)
                {
                        if(strcmp(c1.username, username ) == 0)
                        {	
                                bool booking_allowed = false;
                                
                                for(x=0; x<10; x++)
                                {
                                        if (c1.booking_id[x] == 0)
                                        {
                                                booking_allowed = true;
                                                break;
                                        }
                                }

                                if(!booking_allowed)
                                        return 0;        //Booking Not possible due to access bookings
                        
                                strcpy(c.username, c1.username);
                                strcpy(c.password, c1.password);
                                
                                c.cust_type = c1.cust_type;
                                c.cust_status = c1.cust_status;
                                
                                for(y=0; y<10;y++)
                                {
                                        c.booking_id[y] = c1.booking_id[y];
                                }
                                c.booking_id[x] = bking.booking_id;
                                
                                struct flock lock;
                                lock.l_type = F_WRLCK;
                                lock.l_whence = SEEK_SET;
                                lock.l_start = ((i-1)*1048)+1;
                                lock.l_len = 1048;
                                lock.l_pid = getpid();
                        
                                fcntl(customer_fd, F_SETLKW, &lock);
                                        
                                lseek(customer_fd, ((i-1) * 1048), SEEK_SET);
                                        
                                write(customer_fd, &c, 1048);
                                        
                                lock.l_type = F_UNLCK;
                                fcntl(customer_fd, F_SETLK, &lock);
                                
                                break;
                        }
                        i++;
                }
                close(customer_fd);

                /****Train num to be checked and seats to be uppdated****/
                int train_fd = open("Train.txt", O_CREAT | O_RDWR, 0766); //for Train
                struct Train t1, t;
                i=1;
                
                int seats_available = false;
                
                while(read(train_fd, &t1, sizeof(t)) > 0)
                {
                        if((t1.train_no - bking.train_no == 0) && (t1.total_seats-t1.booked_seats >= bking.seats_booked) && t1.train_status == 1)
                        {
                                seats_available = true;		
                                t.train_no = t1.train_no;
                                t.total_seats = t1.total_seats;
                                t.booked_seats = t1.booked_seats + bking.seats_booked;
                                t.train_status = t1.train_status;
                                        
                                
                                struct flock lock;
                                lock.l_type = F_WRLCK;
                                lock.l_whence = SEEK_SET;
                                lock.l_start = ((i-1)*16)+1;
                                lock.l_len = 16;
                                lock.l_pid = getpid();
                        
                                fcntl(train_fd, F_SETLKW, &lock);
                                        
                                lseek(train_fd, ((i-1) * 16), SEEK_SET);
                                        
                                write(train_fd, &t, 16);
                                        
                                lock.l_type = F_UNLCK;
                                fcntl(train_fd, F_SETLK, &lock);
                                
                                break;

                        }

                        if((t1.train_no - bking.train_no == 0) && t1.train_status == 0)
                        {
                                close(booking_fd);
                                close(train_fd);
                                return 2;
                        }
                        i++;
                }

                /*******Train checked and seats checked **************/
                booking_fd = open("Booking.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
                if(seats_available)
                {
                        struct flock lock;
                        lock.l_type = F_WRLCK;	
                        lock.l_whence = SEEK_CUR;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        fcntl(booking_fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
                        write(booking_fd, &bking, sizeof(bking));
                        
                        lock.l_type = F_UNLCK;			// unlocking.
                        fcntl(booking_fd, F_SETLK, &lock);		// unlocking.
                        
                        close(booking_fd);
                        close(train_fd);
                        
                        return 1; //boooked successfully
                }

                else
                {
                        close(booking_fd);
                        close(train_fd);
                        return 0; //booking unsuccessful bcoz of seats unavailability or train doesn't exits
                }

}


int get_booking_details(struct Booking *book_arr){
                int booking_fd = open("Booking.txt", O_RDONLY, 0766);
                printf("Booking Details\n");

                struct Booking b;
                int i=0;
                
                while(read(booking_fd, &b, sizeof(b))>0)
                {
                        if(b.booking_status == 1)		
                                printf("\n%d\t%s\t%d\t%d", b.booking_id, b.username, b.train_no, b.seats_booked);
                        book_arr[i] = b;
                        i++;
                }

                close(booking_fd);
                return i;
}


int get_booking_details_by_username(struct Booking *arr, char *username){
	int booking_fd = open("Booking.txt", O_RDONLY, 0766);
	
	printf("Booking Details\n");
	struct Booking b;
	int i = 0;
	
	while(read(booking_fd, &b, sizeof(b))>0)
	{
		if(strcmp(b.username, username) == 0){		
			printf("\n%d\t%s\t%d\t%d\t%d", b.booking_id, b.username, b.train_no, b.seats_booked, b.booking_status);
			arr[i] = b;
			i++;
			}
			
		
	}
	close(booking_fd);
	return i;
}


int update_booking(int booking_id, int seats, char *username){
                int booking_fd = open("Booking.txt", O_RDWR, 0766);
                int i=1;
                
                struct Booking b1, b;
                bool update_possible = false;

                while(read(booking_fd, &b1, sizeof(b1)) > 0)
                {		
                        if((booking_id - b1.booking_id == 0) && strcmp(b1.username, username)==0)
                        {
                                update_possible = true;
                                if (b1.booking_status == 0)
                                        return 0; //update not possible for cancelled bookings
                                                
                                b.booking_id = b1.booking_id;
                                b.train_no = b1.train_no;
                                strcpy(b.username, b1.username);
                                b.booking_status = 1;

                                if(seats <= (-1 * b1.seats_booked))
                                {
                                        b.booking_status = 0; //making ticket cancel
                                        seats = -1 * b1.seats_booked; //in this case, all tickets will get cancelled
                                }
                                b.seats_booked = b1.seats_booked + seats;
                                
                                /****Train num to be checked and seats to be uppdated****/
                                int train_fd = open("Train.txt", O_RDWR, 0766); //for Train
                                struct Train t1, t;
                                int j=1;

                                int seats_available = false;
                               
                                while(read(train_fd, &t1, sizeof(t))>0)
                                {
                                        if((t1.train_no - b1.train_no == 0) && (t1.total_seats - t1.booked_seats >= seats))
                                        {
                                                seats_available = true;		
                                                
                                                t.train_no = t1.train_no;
                                                t.total_seats = t1.total_seats;
                                                t.booked_seats = t1.booked_seats + seats;
                                                
                                                if(t.booked_seats < 0)
                                                        t.booked_seats = 0;
                                                t.train_status = t1.train_status;
                                                
                                                struct flock lock;
                                                lock.l_type = F_WRLCK;
                                                lock.l_whence = SEEK_SET;
                                                lock.l_start = ((j-1)*16)+1;
                                                lock.l_len = 16;
                                                lock.l_pid = getpid();
                                        
                                                fcntl(train_fd, F_SETLKW, &lock);
                                                        
                                                lseek(train_fd, ((j-1) * 16), SEEK_SET);
                                                        
                                                write(train_fd, &t, 16);
                                                        
                                                lock.l_type = F_UNLCK;
                                                fcntl(train_fd, F_SETLK, &lock);
                                                break;
                                        }
                                   j++;
                                }
                                
                                if(!seats_available)
                                        return 0; //required no. of seats not available
                                
                                /*******Train checked and seats checked **************/
                                
                                struct flock lock;
                                lock.l_type = F_WRLCK;
                                lock.l_whence = SEEK_SET;
                                lock.l_start = ((i-1)*516)+1;
                                lock.l_len = 516;
                                lock.l_pid = getpid();
                        
                                fcntl(booking_fd, F_SETLKW, &lock);
                                        
                                lseek(booking_fd, ((i-1) * 516), SEEK_SET);
                                        
                                write(booking_fd, &b, 516);
                                        
                                lock.l_type = F_UNLCK;
                                fcntl(booking_fd, F_SETLK, &lock);
                        }
                   i++;
                }
                
                close(booking_fd);
                
                if (!update_possible)
                        return 0;

                return 1;
}


void delete_booking(int booking_id)
{
	int booking_fd = open("booking.txt", O_RDWR, 0766);
	int i=1;
	
	struct Booking b1, b;
	
	while(read(booking_fd, &b1, sizeof(b1))>0)
	{		
		if(booking_id - b1.booking_id == 0)
		{
			
			b.booking_id = b1.booking_id;
			b.train_no = b1.train_no;
			strcpy(b.username, b1.username);
			b.seats_booked = b1.seats_booked;
			b.booking_status = 0;
				
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*516)+1;
			lock.l_len = 516;
			lock.l_pid = getpid();
		
			fcntl(booking_fd, F_SETLKW, &lock);
				
			lseek(booking_fd, ((i-1) * 516), SEEK_SET);
				
			write(booking_fd, &b, 516);
				
			lock.l_type = F_UNLCK;
			fcntl(booking_fd, F_SETLK, &lock);
			
			//**** train seats to be modified *****//
			
                        int train_fd = open("Train.txt", O_RDWR, 0766);
			int i=1;
			
			struct Train t1, t;
			
			while(read(train_fd, &t1, sizeof(t1))>0)
			{		
				if(b.train_no - t1.train_no == 0)
				{
					
					t.train_no = t1.train_no;
					t.total_seats = t1.total_seats;
					t.booked_seats = t1.booked_seats - b.seats_booked;
					t.train_status = t1.train_status;
						
					struct flock lock;
					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_SET;
					lock.l_start = ((i-1)*16)+1;
					lock.l_len = 16;
					lock.l_pid = getpid();
				
					fcntl(train_fd, F_SETLKW, &lock);
						
					lseek(train_fd, ((i-1) * 16), SEEK_SET);
						
					write(train_fd, &t, 16);
						
					lock.l_type = F_UNLCK;
					fcntl(train_fd, F_SETLK, &lock);
				}
				i++;
			}
			
			close(train_fd);
			//*******train seat modified

		}
		i++;
	}
	
	close(booking_fd);
}


void delete_booking_by_user(int booking_id, char* username)
{
                int booking_fd = open("Booking.txt", O_RDWR, 0766);
                int i=1;
                
                struct Booking b1, b;
                
                while(read(booking_fd, &b1, sizeof(b1))>0)
                {		
                        if((booking_id - b1.booking_id == 0) && strcmp(b1.username, username) == 0)
                        {
                                b.booking_id = b1.booking_id;
                                b.train_no = b1.train_no;
                                strcpy(b.username, b1.username);
                                b.seats_booked = b1.seats_booked;
                                b.booking_status = 0;
                                
                                struct flock lock;
                                lock.l_type = F_WRLCK;
                                lock.l_whence = SEEK_SET;
                                lock.l_start = ((i-1)*516)+1;
                                lock.l_len = 516;
                                lock.l_pid = getpid();
                        
                                fcntl(booking_fd, F_SETLKW, &lock);
                                        
                                lseek(booking_fd, ((i-1) * 516), SEEK_SET);
                                        
                                write(booking_fd, &b, 516);
                                        
                                lock.l_type = F_UNLCK;
                                fcntl(booking_fd, F_SETLK, &lock);
                                
                                //**** train seats to be modified *****//
                                int train_fd = open("Train.txt", O_RDWR, 0766);
                                int i=1;
                                
                                struct Train t1, t;
                                
                                while(read(train_fd, &t1, sizeof(t1))>0)
                                {		
                                        if(b.train_no - t1.train_no == 0)
                                        {
                                                t.train_no = t1.train_no;
                                                t.total_seats = t1.total_seats;
                                                t.booked_seats = t1.booked_seats - b.seats_booked;
                                                t.train_status = t1.train_status;
                                                        
                                                struct flock lock;
                                                lock.l_type = F_WRLCK;
                                                lock.l_whence = SEEK_SET;
                                                lock.l_start = ((i-1)*16)+1;
                                                lock.l_len = 16;
                                                lock.l_pid = getpid();
                                        
                                                fcntl(train_fd, F_SETLKW, &lock);
                                                        
                                                lseek(train_fd, ((i-1) * 16), SEEK_SET);
                                                        
                                                write(train_fd, &t, 16);
                                                        
                                                lock.l_type = F_UNLCK;
                                                fcntl(train_fd, F_SETLK, &lock);

                                        }
                                        i++;
                                }
                                
                                close(train_fd);
                                //*******train seat modified
                        }
                        i++;
                }
                
                close(booking_fd);
}


void update_customer_status(char* username, int choice, int status){
                int customer_fd = open("Customer.txt", O_RDWR, 0766);
                int i = 1;

                struct Customer cust;
                struct Customer cust1;

                while(read(customer_fd, &cust1, sizeof(cust1)) > 0)
                {
                        if((strcmp(cust1.username, username) == 0) &&  (cust1.cust_type == choice))
                        {
                                strcpy(cust.username, cust1.username);
                                strcpy(cust.password, cust1.password);

                                for(int j=0; j<10; j++)
                                {
                                        cust.booking_id[j] = cust1.booking_id[j];
                                }

                                cust.cust_type = cust1.cust_type;
                                cust.cust_status = status;

                                struct flock lock;
                                lock.l_type = F_WRLCK;
                                lock.l_whence = SEEK_SET;
                                lock.l_start = ((i-1) * sizeof(struct Customer)) + 1;
                                lock.l_len = sizeof(struct Customer);
                                lock.l_pid = getpid();

                                fcntl(customer_fd, F_SETLKW, &lock);
                                lseek(customer_fd, ((i-1) * sizeof(struct Customer)), SEEK_SET);

                                write(customer_fd, &cust, sizeof(struct Customer));

                                lock.l_type = F_UNLCK;
                                fcntl(customer_fd, F_SETLKW, &lock);
                        }
                    i++;
                }
        close(customer_fd);
}


int delete_customer(char *username, int type)
{
	int customer_fd = open("Customer.txt", O_RDWR, 0766);
	int i=1;
	
	struct Customer c1, c;
	
	while(read(customer_fd, &c1, sizeof(c1)) > 0)
	{		
		if(strncmp(c1.username, username, strlen(c1.username)) == 0 && c1.cust_type == type && c1.cust_status == 1)
		{
			
			strcpy(c.username, c1.username);		// copying from c1.username to c.username
			strcpy(c.password, c1.password);
			int j;
			for(j=0; j<10;j++)
                        {
				c.booking_id[j] = c1.booking_id[j];
				if(c1.booking_id[j] != 0)
					delete_booking(c1.booking_id[j]); //delete corresponding booking details
			}
			c.cust_type = c1.cust_type;
			c.cust_status = 0;
			
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*1048)+1;
			lock.l_len = 1048;
			lock.l_pid = getpid();
		
			fcntl(customer_fd, F_SETLKW, &lock);
				
			lseek(customer_fd, ((i-1) * 1048), SEEK_SET);
				
			write(customer_fd, &c, 1048);
				
			lock.l_type = F_UNLCK;
			fcntl(customer_fd, F_SETLK, &lock);
			close(customer_fd);
			
                        return 1;
		}

		if(strncmp(c1.username, username, strlen(c1.username)) == 0 && c1.cust_type == type && c1.cust_status == 0)
		{
			close(customer_fd);
			return(2);
		}
		i++;
	}
	
	close(customer_fd);
	return 0;
}


void delete_booking_based_train(int train_num)
{
	int booking_fd = open("Booking.txt", O_RDWR, 0766);
	int i = 1;
	
	struct Booking b1, b;
	
	while(read(booking_fd, &b1, sizeof(b1))>0)
	{		
		if(train_num - b1.train_no == 0)
		{
			
			b.booking_id = b1.booking_id;
			b.train_no = b1.train_no;
			strcpy(b.username, b1.username);
			b.seats_booked = b1.seats_booked;
			b.booking_status = 0;
				
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*516)+1;
			lock.l_len = 516;
			lock.l_pid = getpid();
		
			fcntl(booking_fd, F_SETLKW, &lock);
				
			lseek(booking_fd, ((i-1) * 516), SEEK_SET);
				
			write(booking_fd, &b, 516);
				
			lock.l_type = F_UNLCK;
			fcntl(booking_fd, F_SETLK, &lock);
			

		}
		i++;
	}
	
	close(booking_fd);
}


int delete_train(int train_num)
{
	int train_fd = open("Train.txt", O_RDWR, 0766);
	int i = 1;
	
	struct Train t1, t;
	
	while(read(train_fd, &t1, sizeof(t1)) > 0)
	{		
		if(train_num - t1.train_no == 0 && t1.train_status == 1)
		{
			
			t.train_no = t1.train_no;
			t.total_seats = t1.total_seats+t1.booked_seats;
			t.booked_seats = 0;
			t.train_status = 0;
			delete_booking_based_train(train_num); //delete all  bookings in the train	
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*16)+1;
			lock.l_len = 16;
			lock.l_pid = getpid();
		
			fcntl(train_fd, F_SETLKW, &lock);
				
			lseek(train_fd, ((i-1) * 16), SEEK_SET);
				
			write(train_fd, &t, 16);
				
			lock.l_type = F_UNLCK;
			fcntl(train_fd, F_SETLK, &lock);
			close(train_fd);
			
                        return 1;
		}

		if(train_num - t1.train_no == 0 && t1.train_status == 0){
			close(train_fd);
			return 2;
		}
		i++;
	}
	
	close(train_fd);
	return 0;
}


int resume_train(int train_num)
{
	int train_fd = open("Train.txt", O_RDWR, 0766);
	int i = 1;
	
	struct Train t1, t;
	
	while(read(train_fd, &t1, sizeof(t1))>0)
	{		
		if(train_num - t1.train_no == 0 && t1.train_status == 0)
		{
			t.train_no = t1.train_no;
			t.total_seats = t1.total_seats;
			t.booked_seats = 0;
			t.train_status = 1;
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*16)+1;
			lock.l_len = 16;
			lock.l_pid = getpid();
		
			fcntl(train_fd, F_SETLKW, &lock);
				
			lseek(train_fd, ((i-1) * 16), SEEK_SET);
				
			write(train_fd, &t, 16);
				
			lock.l_type = F_UNLCK;
			fcntl(train_fd, F_SETLK, &lock);
			
                        return 1;
		}

		if(train_num - t1.train_no == 0 && t1.train_status == 1)
			return 2;
		i++;
	}
	
	close(train_fd);
	return 0;
}


int add_agent(struct Agent cstmr){
	int agent_fd = open("Agent.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	struct Agent c1;
	int agent_exists = false;
	
        while(read(agent_fd, &c1, sizeof(c1))>0)
	{
		if( strcmp(c1.username, cstmr.username) == 0)
		{
			agent_exists = true;
			break;
		}
	}

	if(!agent_exists)		// username doesnt exist
	{
		printf("\nunder not flag\n");
		struct flock lock;
		
                lock.l_type = F_WRLCK;	
		lock.l_whence = SEEK_CUR;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();
		
                fcntl(agent_fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
		write(agent_fd, &cstmr, sizeof(cstmr));
		
                lock.l_type = F_UNLCK;			// unlocking.
		fcntl(agent_fd, F_SETLK, &lock);		// unlocking.
		
		close(agent_fd);
		
		// -----------locking and writing into customer file-------------------
		
		return 1;
	}
	
        else   // username exists
        {
		close(agent_fd);
		return 0;
        }

	close(agent_fd);
}


int add_booking_agent(struct Booking bking, char *username){
	int booking_fd = open("Booking.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	/******counting total number of bookings*************/
	int count = 0;
	struct Booking b1;
	while(read(booking_fd, &b1, sizeof(b1))>0)
		count++;

	bking.booking_id = 1000 + count + 1;
	close(booking_fd);

	/****User details to be checked and modified*************/
	int agent_fd = open("Agent.txt", O_RDWR, 0766); //for user i.e customer
	struct Customer c1;
	struct Customer c;
	int i=1;
	int x, y;
	while(read(agent_fd, &c1, sizeof(c1))>0)
	{
		if(strcmp(c1.username, username)==0)
		{	
			bool seats_available = false;
			for(x=0; x<10; x++){
				if (c1.booking_id[x]==0){
					seats_available = true;
					break;
				}
			}
			if(!seats_available)
				return 0; //Booking Not possible due to access bookings
		
			strcpy(c.username, c1.username);
			strcpy(c.password, c1.password);
			c.cust_type = c1.cust_type;
			c.cust_status = c1.cust_status;
			
			for(y=0; y<10;y++){
				c.booking_id[y] = c1.booking_id[y];
			}
			c.booking_id[x] = bking.booking_id;
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*1048)+1;
			lock.l_len = 1048;
			lock.l_pid = getpid();
		
			fcntl(agent_fd, F_SETLKW, &lock);
				
			lseek(agent_fd, ((i-1) * 1048), SEEK_SET);
				
			write(agent_fd, &c, 1048);
				
			lock.l_type = F_UNLCK;
			fcntl(agent_fd, F_SETLK, &lock);
			break;

		}
		i++;
	}
	close(agent_fd);
	/********************************************************/
	
	
	/****Train num to be checked and seats to be uppdated****/
	int train_fd = open("Train.txt", O_CREAT | O_RDWR, 0766); //for Train
	struct Train t1, t;
	i=1;
	
	int seats_available = false;
	while(read(train_fd, &t1, sizeof(t))>0)
	{
		if((t1.train_no - bking.train_no == 0) && (t1.total_seats-t1.booked_seats>=bking.seats_booked) && t1.train_status == 1)
		{
			seats_available = true;		
			t.train_no = t1.train_no;
			t.total_seats = t1.total_seats;
			t.booked_seats = t1.booked_seats + bking.seats_booked;
			t.train_status = t1.train_status;
				
			
			struct flock lock;
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = ((i-1)*16)+1;
			lock.l_len = 16;
			lock.l_pid = getpid();
		
			fcntl(train_fd, F_SETLKW, &lock);
				
			lseek(train_fd, ((i-1) * 16), SEEK_SET);
				
			write(train_fd, &t, 16);
				
			lock.l_type = F_UNLCK;
			fcntl(train_fd, F_SETLK, &lock);
			
                        break;

		}
		if((t1.train_no - bking.train_no == 0)&& t1.train_status == 0)
		{
			close(booking_fd);
			close(train_fd);
			return 2;
		}
		i++;
	}
	/*******Train checked and seats checked **************/
	booking_fd = open("Booking.txt", O_CREAT | O_RDWR | O_APPEND, 0766);
	
        if(seats_available){
		struct flock lock;
		lock.l_type = F_WRLCK;	
		lock.l_whence = SEEK_CUR;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();
		
                fcntl(booking_fd, F_SETLKW, &lock);		// locking.  -> here on critical section.
		write(booking_fd, &bking, sizeof(bking));
		lock.l_type = F_UNLCK;			// unlocking.
		fcntl(booking_fd, F_SETLK, &lock);		// unlocking.
		
		close(booking_fd);
		close(train_fd);
		return 1; //boooked successfully
	}

	else
        {
		close(booking_fd);
		close(train_fd);
		return 0; //booking unsuccessful bcoz of seats unavailability or train doesn't exits
	}
}


int is_user_agent_valid(char *name)
{
	int agent_fd = open("Agent.txt", O_RDONLY, 0766);
		
	struct Agent a;
		
	while(read(agent_fd, &a, sizeof(a))>0)
	{		
		if(strcmp(a.username, name) == 0 && a.agent_status == 1)
			return 1;
	}
	close(agent_fd);
	return 0;
}


int is_agent_password_valid(char *name, char *pass)
{
	int agent_fd = open("Agent.txt", O_RDONLY, 0766);
	int value;
	
	struct Agent a;
	
	while(read(agent_fd, &a, sizeof(a))>0)
	{		
		if(strcmp(a.username, name) == 0 && strcmp(a.password, pass) == 0)
		{
			value = a.agent_type + 10;
			printf("value of value %d\n", value);
			return value;
		}
	}
	close(agent_fd);
	return (0);
}
