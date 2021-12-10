# DEU HOSPITAL SIMULATIONS

In this assignment, you are asked to create a simulation of a hospital using POSIX
Threads, Mutexes and Semaphores. Your hospital should have the following facilities for
patients:

# Requirements

* GNU Compiler Collection (gcc) on running linux.
* If you have linux Dockerfile already created.

## installation with docker
First 
```
$ docker build . -t container_name 
```
Then
```
$ docker run -v --rm -it container_name
```

## installation with docker
First
```
$ gcc main.c -lpthread

or

$ gcc -o runnable main.c -lpthread -lrt
```
Then
```
$ ./runnable
```

# Record Patient

```
typedef struct PatientStruct {
    int patientId;
    int hungerScore;
    int toiletScore;
    int disease;
    pthread_t pthreadId;
} Patient;
```
## Section or Departments

**Registration:** A registration office for patients to register and forwarded to the correct
department for treatment. The size of registration office should be stored in a global
integer called “REGISTRATION_SIZE”.

**Restroom:** A restroom that are available for patients to use. The size of restroom should
be stored in a global integer called “RESTROOM_SIZE”.

**Cafe:** A place where patients can stay while waiting for an operation to finish in hospital.
The cafe should contain a number of cashiers that can serve simultaneously and this
number should be stored in a global integer called “CAFE_NUMBER”.

**General Practitioner (GP):** A standard doctor that listens to patients complaints and
requests additional tests, prescribe medicine or forwards them to a specific department.
The total number of GP doctors should be stored in a global integer called
“GP_NUMBER”.

**Pharmacy:** A place that patients go to buy medicine that is prescribed by GP doctors.
The pharmacy should contain a number of cashiers that can serve simultaneously and this
number should be stored in a global integer called “PHARMACY_NUMBER”.
**Blood Lab:** A place were patient’s bloods are taken to be analyzed on the request of GP
doctors. The blood lab should contain a number of stations, manned by assistants to
collect blood from patients. The number of these stations should be stored in a global
integer called “BLOOD_LAB_NUMBER”.

**Surgery:** A place that patients requiring surgery are operated on. They are diagnosed and
prescribed a surgery by GP doctors. A surgery is done by one or more surgeons, one or
more nurses and inside an operating room (OR). The patient should wait until required
number of surgeons, nurses and an OR are available to do the operation. Be careful
because this could lead to a dead-lock situation. The numbers of surgeons, nurses and
operating theaters should be stored in global integers called “SURGEON_NUMBER”,
“NURSE_NUMBER” and “OR_NUMBER”.

## Basic scenario of simulation

The patients should be constructed using POSIX Threads and contain the required
properties by the program. The patients should follow the given algorithm below:
The patient is generated with a specific disease that requires either medicine or surgery.
The patient goes to registration office and waits for an available desk to register.
After registration, the patient goes to GP’s office and examined.
After examination the patient is send to one of the three main departments.
If the patient is prescribed medicine by GP, they go to pharmacy and buy this drug,
waiting for an empty register if there is none.
If the patient is asked to give blood for testing in blood lab, the patient goes to blood lab,
waits for an available assistant and gives blood.
After given blood, the patient returns to GP and they are either prescribed medicine if
they need it and goes to pharmacy as has been shown above or not if they do not require
it.
If the patient requires a surgery, patient waits for an operating room, a randomly selected
number or surgeons and nurses to be available and after all of them are acquired, the
surgery is performed. After surgery, the patient goes to GP and they are either prescribed
medicine and goes to pharmacy as has been shown above or not if they do not require it.
During any of waiting period of previously given operations, if the patient is sufficiently
hungry or needs go to a toilet, they can buy food from hospital cafe or go to the hospital
restroom. At the end of every waiting period, these hunger and restroom meters are
increased by a random value between 1 and given global increase rates.
Your program should print every operation or event that happens inside it. It should be in
orderly and informative format. Please create your own console output format
individually and according to what you think is the best.
Because of randomization of costs, waiting times and requirement number of surgeons
and nurses for surgeries, every execution of your program should be unique. If it is
outputting the same results every time, there must be something wrong in your program.

## todo list

|macro|description|status|
|-|-|:-:|
|`getting terminal arguments`|All procces can be export with for csv file|:soon:|
|`another type output`|Program should be creatable different outputs (pdf,csv)|:soon:|
|`another type output`|Program should be runnable different operating system.|:soon:|