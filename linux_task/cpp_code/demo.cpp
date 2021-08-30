#include<iostream>
using namespace std;
class A
{
private:
    /* data */
    int x;
    int y;

    void change_x()
    {
        cout<<"change x"<<endl;
    }
public:
int z;
void c_x()
{
   change_x(); 
}
A()
{
    this->x=0;
    this->y=0;
}
    A(int x,int y)
    {
        this->x = x;
        this->y = y;
    }
   virtual void disp()
    {
        cout<<"Iam A x"<<x<<"y"<<y<<endl;
    }

    void operator+=(const A object)
    {
        this->x += object.x;
        this->y += object.y;
    }
};

 class B: public A
{
private:
    /* data */
public:
    void disp()
    {
          cout<<"Iam B "<<endl;
    }
        void disp2()
    {
        cout<<"Iam B2"<<endl;
    }
};

int x=10;

int main()
{
    int x=20;
    cout<< "local"<<x<<endl;
    cout<< "global"<<::x<<endl;
 #if 0
    A obj1(1,2),obj2(3,4);
   // B obj2;
    obj1.disp();
    obj2.disp();
    obj1+=obj2;
    obj1.disp();
    obj2.disp();
#endif
#if 0
B obj_b;
obj_b.c_x();
obj_b.z;
#endif
#if 0
A *base_ptr;
B obj_b;
base_ptr = &obj_b;
base_ptr->disp();
static_cast < B *>(base_ptr)->disp2();
(( B * )(base_ptr) )->disp2();
#endif

}