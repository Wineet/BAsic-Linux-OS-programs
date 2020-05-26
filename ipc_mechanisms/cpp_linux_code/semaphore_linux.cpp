/*
 * Author:   vinit 
 * Task:     Creating Linux Semaphore and locking to avoide race condition
 * 				Linux Task5 
 * Hw  : 	   R-pi 3B+
 * Platform: Linux
 *
 */


#include<iostream>
#include<cstring>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<pthread.h>
#include<unistd.h>

#define PAGE_SIZE 4096

using namespace std;

#if 0
/*This two structres are internally Defined in Header File
 * sys/ipc.h  File*/

	struct semid_ds {
               struct ipc_perm sem_perm;  /* Ownership and permissions */
               time_t          sem_otime; /* Last semop time */
               time_t          sem_ctime; /* Last change time */
               unsigned long   sem_nsems; /* No. of semaphores in set */
           };

           struct ipc_perm {
               key_t          __key; /* Key supplied to semget(2) */
               uid_t          uid;   /* Effective UID of owner */
               gid_t          gid;   /* Effective GID of owner */
               uid_t          cuid;  /* Effective UID of creator */
               gid_t          cgid;  /* Effective GID of creator */
               unsigned short mode;  /* Permissions */
               unsigned short __seq; /* Sequence number */
	   };
#endif
	union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };

class Semaphore{

	private: 

	int ipc_key;		// ipc_key
	int sem_id;		// Holds semaphore group id
	int nsem;		// Number of semaphore
	union semun sem_data;
	struct sembuf sem_buf;	// sembuf for semaphore operation semop

	/* Function implement in this class
	 *
	 * semaphore constructor with arguments
	 * semaphore create
	 * semaphore set initial value
	 * semaphore set IPC key
	 * semaphore lock
	 * semaphore unlock
	 * get semaphore permission
	 * get semaphore count
	 * semaphore destory
	 *
	 * sem_lock		// used for binary Semaphore
	 * sem_unlock		//used for binary semaphore
	 * */

	/**/
	public:
	/*Constructor of Semaphore Class */
	
	Semaphore(int n_sem=1,int ipc_key=3333)
	{
		/* creates binary Semaphore with initial value = 1
		 * For locking Semaphore can be used directly
		 * semaphore constructor with no Argument
		 * */
		this->ipc_key   = ipc_key;		
		this->nsem 	= n_sem;
		sem_create();			
		set_sema_val();
	
		
	}

	void sem_create(int n_sem=1,int ipc_key=3333);
	void sem_lock(int sem_num=0);		// sem lock is used to set vale 
	int  get_sema_val(int sem_no=0);
	void set_sema_val(int sem_no=0,int sem_val=1);
	void sem_unlock(int sem_num=0);
	void sem_destroy();	
	
	/*
	 * Inside class Defined Functions are taken as Inline function
	 * int get_ipc_key();
	 * int get_nsem();
	 * int get_sem_id();
	 *
	 */
	int get_ipc_key()
	{
		return ipc_key;
	}
	int get_nsem()
	{
		return nsem;
	}
	int get_sem_id()
	{
		return sem_id;
	}

};

class Mutex{
	
	private:
	pthread_mutex_t m_lock;
	/*Mutex Class is created for Acquiring Mutex Lock*/
	
	public:
	Mutex()
	{
		if(0 != pthread_mutex_init(&m_lock,NULL))
		{
			cout<<"Mutex_init Failed\n"<<endl;
		}
	}
	void mutex_lock()
	{
			 pthread_mutex_lock(&m_lock);
	}
	void mutex_unlock()
	{
			 pthread_mutex_unlock(&m_lock);
	}
	void mutex_destroy()
	{
		pthread_mutex_destroy(&m_lock);
	}
};
class Shared_mem{

	private:
	int ipc_key;
	int no_page;
	int shm_id;
	
	
	/* Function implmented in this Class as below
	 *
	 * Create Memory Constructor shared memory Size as Argument 
	 * 
	 * Shared Memory Size
	 * Destroy Shared Memory
	 * Whatsize
	 * */
	
	 public:
	 /*Shared Memory access pointer is needed*/
		void *shared_mem_ptr = NULL;

	/*
	 *	shared Memory class constructor
	 *  By default allocate one page of shared memory
	 */
	 Shared_mem(int no_page=1,int ipc_key= 2222)
	 {
		 shm_id = shmget((key_t)ipc_key,no_page*PAGE_SIZE, 0777|IPC_CREAT);
			if(shm_id == -1)
			{
			  cout<<"Error in shm_id\n"<<strerror(errno)<<endl;
			}	
				this->ipc_key = ipc_key;
				this->no_page = no_page;
				this->shm_id  = shm_id;
					shared_mem_ptr =  shmat(shm_id,(void *)0,0);	// Read write both acess

			if( (void *)-1 == shared_mem_ptr)
			{
			  cout<<"Error in shmat "<<strerror(errno)<<endl;
			}
	 }
	 void detach_shared_mem();
	 void destory_shared_mem();
	
		 int get_size()
		{
			return no_page*PAGE_SIZE;
		}
		
		int get_ipc_key()
		{
			return ipc_key;
		}
		
};

/*Global Variable Here*/

/* Function prototype */
void *inc_thread(void *arg_ptr);
void *dec_thread(void *arg_ptr);

/* Global structre Define Here*/
typedef struct {

	long int global_var1 ;
	long int global_var2 ;
	long int global_var3 ;

}shared_struct;

	struct mut_shm{
					shared_struct *shm_obj_ptr;
					Mutex *m_lock;
				};




/*Global structre Definition End Here*/


int  main(int argc,char *argv[])
{
	
	int ret_val=0;
/*Child Pid*/
	pid_t child_pid1=0;
	pid_t child_pid2=0;
/* Default constructor creates a binary semaphore
*	with initial set value as 1 (unlocked Semaphore)
*  Default IPC key 3333
*/
	Semaphore sema;		
/*
*	Default constructor creates shared memory of page size One
*	with Default IPC key 2222 
*	shared_mem_ptr
*	pointer has shared memory address
*
*/
	Shared_mem shm;		
/*
* structure holds three global varible which need to be managed and changed
*
*/
	shared_struct *shm_obj_ptr;
	shm_obj_ptr = (shared_struct *)shm.shared_mem_ptr;
	/* Default Value initilizing of global varibale*/
	shm_obj_ptr->global_var1= 10;
	shm_obj_ptr->global_var2= 12;
	shm_obj_ptr->global_var3= 14;
	
	/* creating two Child Here 
	 * For shared Memory resource Used
	 * Race condition will be avoided by Semaphore Locks
	 */
		
		if( 0==(child_pid1=fork()))
		{
			cout<<" child 1 created "<<endl;
			
				int ret_val=0;
				pthread_t thread1;
				pthread_t thread2;
				Mutex m_lock;
				
				struct mut_shm mut_shm_s;
				mut_shm_s.m_lock=&m_lock;
				mut_shm_s.shm_obj_ptr =shm_obj_ptr;
				
			/*Child 1 operational Code Here*/
			sema.sem_lock();
			
			if(0 != pthread_create(&thread1,NULL,inc_thread,&mut_shm_s))
			{
				cout<<"pthread Error \n"<<strerror(errno)<<endl;
				exit(EXIT_FAILURE);
			}
			if(0 != pthread_create(&thread2,NULL,dec_thread,&mut_shm_s))
			{
				cout<<"pthread Error\n"<<strerror(errno)<<endl;
				exit(EXIT_FAILURE);
			}
	
			pthread_join(thread1,NULL);
			pthread_join(thread2,NULL);
		
			sema.sem_unlock();
				m_lock.mutex_destroy();
			exit(EXIT_SUCCESS);
			
		   /*Exit here with return status so child will not interrupt further*/
		}

		if( 0==(child_pid2=fork()))
		{
			cout<<" child 2 created "<<endl;
			/*Child 2 operational Code Here*/
				int ret_val=0;
				pthread_t thread1;
				pthread_t thread2;
				Mutex m_lock;
			
				struct mut_shm mut_shm_s;
				mut_shm_s.m_lock=&m_lock;
				mut_shm_s.shm_obj_ptr =shm_obj_ptr;
				
			sema.sem_lock();
					
				if(0 != pthread_create(&thread1,NULL,inc_thread,&mut_shm_s))
				{
					cout<<"pthread Error \n"<<strerror(errno)<<endl;
					exit(EXIT_FAILURE);
				}
				if(0 != pthread_create(&thread2,NULL,dec_thread,&mut_shm_s))
				{
					cout<<"pthread Error\n"<<strerror(errno)<<endl;
					exit(EXIT_FAILURE);
				}
	
			pthread_join(thread1,NULL);
			pthread_join(thread2,NULL);

					sema.sem_unlock();
			m_lock.mutex_destroy();
			exit(EXIT_SUCCESS);
			/*Exit here with return status so child will not interrupt further*/
		}
	
	/*using wait API for waiting purpose until CHild Finishes there work
	 * wait PID not using return Failure is not possible so
	 * And Child Returns Nothing 
	 */
	
	wait(&ret_val);
	wait(&ret_val);
	
	cout<<"shm_obj_ptr->global_var1 = "<<shm_obj_ptr->global_var1<<endl;
	cout<<"shm_obj_ptr->global_var2 = "<<shm_obj_ptr->global_var2<<endl;
	cout<<"shm_obj_ptr->global_var3 = "<<shm_obj_ptr->global_var3<<endl;
	
	/* Destroy Created Semaphore
	 * Clearing Semaphore Resource
	 */
	 cout<<"Clearing Resources"<<endl;
	sema.sem_destroy();
	
	/*Memory is Detached and it will be get 
	* Destroyed when last process will detach form
	* Shared Memory
	*/
	shm.destory_shared_mem();

return 0;
}



/*Semapahore Class Function Definition Here*/
/*
	void semaphore::sem_create(int n_sem=1,int ipc_key=3333);
	void semaphore::sem_lock(int sem_num=0);		// sem lock is used to set vale 
	int semaphore::get_sema_val(int sem_no=0);
	void semaphore::set_sema_val(int sem_no=0,int sem_val=1);
	void semaphore::sem_unlock(int sem_num=0);
	void semaphore::sem_destroy();
*/
	void Semaphore::sem_create(int n_sem,int ipc_key)
	{
		/* Used to set/get(If already created)
		 *
		 * Identifier Associated with Key*/
		sem_id=semget((key_t)ipc_key,n_sem,IPC_CREAT|0777);	//Permission to all
		if( -1 == sem_id)
		{
			cout<<"semget Error"<<strerror(errno)<<endl;
		}
		this->sem_id = sem_id;
	}

	
	int Semaphore::get_sema_val(int sem_no)
	{
		int sem_val=0;

		sem_val=semctl(sem_id,sem_no,GETVAL,sem_data);
		if(-1 == sem_val)
		{
			cout<<"get sem_val error "<<strerror(errno)<<endl;
		}
		return sem_val;
	}
	

	void Semaphore::set_sema_val(int sem_no,int sem_val)
	{
	
		sem_data.val=sem_val;			
		sem_val=semctl(sem_id,sem_no,SETVAL,sem_data);
		if(-1 == sem_val)
		{
			cout<<"get sem_val error "<<strerror(errno)<<endl;
		}
	
	}
	void Semaphore::sem_lock(int sem_num)			// sem lock is used to set vale 
	{
		sem_buf.sem_num = sem_num;
		sem_buf.sem_op  = -1;
		sem_buf.sem_flg = SEM_UNDO;
		if(0 != semop(sem_id,&sem_buf,1))	//operation As on Binary Semaphore Consideration
		{
			cout<<"semOp Error"<<strerror(errno)<<endl;
		}	
	}
	void Semaphore::sem_unlock(int sem_num)
	{
		sem_buf.sem_num = sem_num;
		sem_buf.sem_op  = +1;
		sem_buf.sem_flg = SEM_UNDO;
		if(0 != semop(sem_id,&sem_buf,1))	//operation As on Binary Semaphore Consideration
		{
			cout<<"semOp Error"<<strerror(errno)<<endl;
		}	
	
	
	}

	void Semaphore::sem_destroy()
	{
		int sem_val=0;
		sem_val=semctl(sem_id,0,IPC_RMID);	// Destroy group of semaphore and unblock blocked processes
		if(-1 == sem_val)
		{
			cout<<"sem_destroy sem_val error "<<strerror(errno)<<endl;
		}
		
	}


/*
 * Semaphore class Definition Ends Here
 *
 */


/*
* Shared_mem class Function Definition
*/

	 void Shared_mem::detach_shared_mem()
	 {
		 	if(0 != shmdt(shared_mem_ptr))
			{
				cout<<"Error While Detaching Memory\n"<<endl;
			}
	 }
	 
	 void Shared_mem::destory_shared_mem()
	 {
			detach_shared_mem();
			cout<<"Deleting SHared Memory\n"<<endl;

			if(0  != shmctl(shm_id,IPC_RMID,0))
			{
				cout<<"shmctl Error..\n"<<endl;
			}
		 
	 }

/*
* shared memory class Function Definition Ends Here
*/


/* Thread Functions */

void *inc_thread(void *arg_ptr)
{
		cout<<" Inc thread created "<<endl;
		struct mut_shm *mut_shm_ptr = (struct mut_shm *)arg_ptr;
	
 for(long int i=0;i<60000;i++ )
 {
	  mut_shm_ptr->m_lock->mutex_lock();
			
		mut_shm_ptr->shm_obj_ptr->global_var1++;
		mut_shm_ptr->shm_obj_ptr->global_var2++;
		mut_shm_ptr->shm_obj_ptr->global_var3++;
					
	 mut_shm_ptr->m_lock->mutex_unlock();

 }

return NULL;
}

void *dec_thread(void *arg_ptr)
{
	cout<<" dec thread created "<<endl;
	struct mut_shm *mut_shm_ptr = (struct mut_shm *)arg_ptr;
	
 for(long int i=0;i<60000;i++ )
 {	
	 mut_shm_ptr->m_lock->mutex_lock();
		mut_shm_ptr->shm_obj_ptr->global_var1--;
		mut_shm_ptr->shm_obj_ptr->global_var2--;
		mut_shm_ptr->shm_obj_ptr->global_var3--;
	 mut_shm_ptr->m_lock->mutex_unlock();
 }

 return NULL;
}

/* End of Thread Functions */