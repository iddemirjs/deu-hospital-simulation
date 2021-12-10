#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

// The number of registration desks that are available.
int REGISTRATION_SIZE = 2; // @@@It had been changed by me.
// The number of restrooms that are available.
int RESTROOM_SIZE = 2; // @@@It had been changed by me.
// The number of General Practitioner that are available.
int GP_NUMBER = 2; // @@@It had been changed by me.
// The number of cashiers in cafe that are available.
int CAFE_NUMBER = 2;
// The number of cashiers in pharmacy that are available.
int PHARMACY_NUMBER = 10;
// The number of assistants in blood lab that are available.
int BLOOD_LAB_NUMBER = 10;
// The number of operating rooms, surgeons and nurses that are available.
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;
/* The maximum number of surgeons and nurses that can do a surgery. A random value is
calculated for each operation between 1 and given values to determine the required
        number of surgeons and nurses. */
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;
// The number of patients that will be generated over the course of this program.
int PATIENT_NUMBER = 10;
// The account of hospital where the money acquired from patients are stored.
int HOSPITAL_WALLET = 0;

int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;
int MAX_HUNGER_RESTROOM_SCORE = 100;
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;
int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; // Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; // Calculated randomly between 1 and given value.

/**
 * Semaphore definitions
 */
sem_t registration_sem;
sem_t generalPractitioner_sem;
sem_t restroom_sem;
sem_t cafe_sem;
sem_t pharmacy_sem;
sem_t bloodLab_sem;
sem_t surgery_sem;

/**
 * Type definiton of patients
 * disease : 0 => healthy, 1 => surgical, 2 => medical, 3 => blood lab
 */
typedef struct PatientStruct {
    int patientId;
    int hungerScore;
    int toiletScore;
    int disease;
    pthread_t pthreadId;
} Patient;

/*
 * A function declaration tells the compiler about a function name and how to call the function.
 * The actual body of the function can be defined separately.
 */
void *registration(void *patient);
int randomNumber(int min, int max);
void createPatients(Patient patientsArg[]);
void toiletUpdater(Patient *patient);
void hungerUpdater(Patient *patient);
void managerRestroomHunger(Patient *patient);
void displayPatientInfo(Patient *patient);
void generalPractitioner(Patient *patient);
void pharmacy(Patient *patient);
void bloodLab(Patient *patient);
void surgery(Patient *patient);
void sectionAvailabilityWaiter(sem_t *waited_sem, Patient *patient,char department[]);

int main(void) {
    srand ( time(NULL) );
    Patient patients[PATIENT_NUMBER];
    createPatients(patients);
    return 0;
}
/**
 * Registration department function
 * @param patient
 * @return
 */
void *registration(void* patient){
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&registration_sem,comingPatient,"registration");
    sem_wait(&registration_sem);
    usleep(1000*(randomNumber(1,REGISTRATION_TIME)));
    printf("Registered patient number : %d\n",comingPatient->patientId);
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    displayPatientInfo(comingPatient);
    HOSPITAL_WALLET += REGISTRATION_COST;
    printf("    -New hospital budget is : %d\n", HOSPITAL_WALLET);
    sem_post(&registration_sem);
    managerRestroomHunger(comingPatient);
    generalPractitioner(comingPatient);
}
/**
 * Cafe function for hungering
 * @param patient
 * @return
 */
void *cafe(void* patient ){
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&cafe_sem,comingPatient,"cafe");
    sem_wait(&cafe_sem);
    usleep(1000*(randomNumber(1,CAFE_TIME)));
    HOSPITAL_WALLET += CAFE_COST;
    toiletUpdater(comingPatient);
    comingPatient->hungerScore = 0;
    printf("*After Cafe visiting hunger score of Patient %d : %d\n",comingPatient->patientId,comingPatient->hungerScore);
    displayPatientInfo(comingPatient);
    sem_post(&cafe_sem);
    managerRestroomHunger(comingPatient);
}
/**
 * Restroom function for toilet requiring.
 * @param patient
 * @return
 */
void *restroom(void* patient){
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&restroom_sem,comingPatient,"restroom");
    sem_wait(&restroom_sem);
    usleep(1000*(randomNumber(1,RESTROOM_TIME)));
    hungerUpdater(comingPatient);
    comingPatient->toiletScore = 0;
    printf("*After Restroom visiting toilet score of Patient %d : %d\n",comingPatient->patientId,comingPatient->toiletScore);
    displayPatientInfo(comingPatient);
    sem_post(&restroom_sem);
    managerRestroomHunger(comingPatient);
}
/**
 * Function that executes the inspection process
 * @param patient
 */
void generalPractitioner(Patient *patient) {
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&generalPractitioner_sem,comingPatient,"general practitioner");
    sem_wait(&generalPractitioner_sem);
    printf("General Practitioner took care of Patient %d\n",comingPatient->patientId);
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    displayPatientInfo(comingPatient);
    usleep(1000*(randomNumber(1,GP_TIME)));
    sem_post(&generalPractitioner_sem);
    managerRestroomHunger(comingPatient);
    if (comingPatient->disease == 0){
        printf("The patient is healthy. Because of that Patient %d left the hospital.\n",patient->patientId);
    }else if (comingPatient->disease == 1) {
        surgery(patient);
    }else if (comingPatient->disease == 2) {
        pharmacy(comingPatient);
    }else if (comingPatient->disease == 3) {
        bloodLab(comingPatient);
    }
}
/**
 * function that executes the surgery process
 * @param patient
 */
void surgery(Patient *patient) {
    Patient *comingPatient = &(*(Patient*) patient);
    int needSurgeon = randomNumber(1,SURGEON_LIMIT);
    int needNurse = randomNumber(1,NURSE_LIMIT);
    while(1){
        printf("Needs op surgeon %d,Needs op nurse : %d \n",needSurgeon,needNurse);
        if(sem_trywait(&surgery_sem)==0 && needSurgeon <= SURGEON_NUMBER && needNurse <= NURSE_NUMBER)
        {
            printf("Patient %d ready to operation.\n",comingPatient->patientId);
            sem_post(&surgery_sem);
            SURGEON_NUMBER-=needSurgeon;
            NURSE_NUMBER-=needSurgeon;
            break;
        }
        else{
            printf("Patient %d is waiting on OR door.\n",comingPatient->patientId);
            usleep(1000*WAIT_TIME);
            hungerUpdater(comingPatient);
            toiletUpdater(comingPatient);
        }
    }
    sem_wait(&surgery_sem);
    usleep(1000*(randomNumber(1,SURGERY_TIME)));
    HOSPITAL_WALLET += (needSurgeon * SURGERY_SURGEON_COST + needNurse * SURGERY_NURSE_COST) + SURGERY_OR_COST;
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    printf("Surgery process for Patient %d then Hospital wallet is : %d\n",comingPatient->patientId,HOSPITAL_WALLET);
    displayPatientInfo(comingPatient);
    sem_post(&surgery_sem);
    SURGEON_NUMBER+=needSurgeon;
    NURSE_NUMBER+=needSurgeon;
    managerRestroomHunger(comingPatient);
}
/**
 * Function in which blood tests are carried out
 * @param patient
 */
void bloodLab(Patient *patient) {
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&bloodLab_sem,comingPatient,"general practitioner");
    sem_wait(&bloodLab_sem);
    usleep(1000*(randomNumber(1,BLOOD_LAB_TIME)));
    HOSPITAL_WALLET += BLOOD_LAB_COST;
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    printf("Blood lab take a blood for testing from Patient %d\n",comingPatient->patientId);
    displayPatientInfo(comingPatient);
    patient->disease = randomNumber(0,2);
    if (patient->disease == 0){
        printf("The patient is healthy now left here.\n");
    }
    sem_post(&bloodLab_sem);
    managerRestroomHunger(comingPatient);
    if (patient->disease != 0) {
        generalPractitioner(comingPatient);
    }
}
/**
 * The place where drugs are collected for drug treatments
 * @param patient
 */
void pharmacy(Patient *patient) {
    Patient *comingPatient = &(*(Patient*) patient);
    sectionAvailabilityWaiter(&pharmacy_sem,comingPatient,"pharmacy");
    sem_wait(&pharmacy_sem);
    usleep(1000*(randomNumber(1,PHARMACY_TIME)));
    HOSPITAL_WALLET += randomNumber(0,PHARMACY_COST);
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    printf("Pharmacy gave a medicine to Patient %d and Patient %d left hospital.\n",
           comingPatient->patientId,comingPatient->patientId);
    displayPatientInfo(comingPatient);
    sem_post(&pharmacy_sem);
    managerRestroomHunger(comingPatient);
}

/**
 * Patient waiter on a department
 * @param waited_sem
 * @param patient
 * @param department
 */
void sectionAvailabilityWaiter(sem_t *waited_sem, Patient *patient,char department[]) {
    Patient *comingPatient = &(*(Patient*) patient);
    while(1){
        if(sem_trywait(&(*(sem_t*) waited_sem)) != 0)
        {
            printf("Patient %d is waiting on %s door.\n",comingPatient->patientId,department);
            usleep(1000*WAIT_TIME);
            hungerUpdater(comingPatient);
            toiletUpdater(comingPatient);
        }
        else{
            printf("Patient %d ready to %s.\n",comingPatient->patientId,department);
            sem_post(&(*(sem_t*) waited_sem));
            break;
        }
    }
};
/**
 * Patient info displayer
 * @param patient
 */
void displayPatientInfo(Patient *patient){
    Patient *comingPatient = &(*(Patient*) patient);
    printf("Patient %d : { hungerScore : %d, toiletScore : %d, disease : %d }\n",
           comingPatient->patientId,
           comingPatient->hungerScore,
           comingPatient->toiletScore,
           comingPatient->disease);
}
/**
 * Toilet and hungering manager
 * @param patient
 */
void managerRestroomHunger(Patient *patient) {
    Patient *comingPatient = &(*(Patient*) patient);
    toiletUpdater(comingPatient);
    hungerUpdater(comingPatient);
    if (comingPatient->hungerScore >= MAX_HUNGER_RESTROOM_SCORE) {
        cafe(comingPatient);
    }
    if (comingPatient->toiletScore >= MAX_HUNGER_RESTROOM_SCORE) {
        restroom(comingPatient);
    }
}
/**
 * Randomly toilet increaser
 * @param patient
 */
void toiletUpdater(Patient *patient){
    patient->toiletScore += randomNumber(1,RESTROOM_INCREASE_RATE);
}
/**
 * Randomly hungering increaser
 * @param patient
 */
void hungerUpdater(Patient *patient){
    patient->hungerScore += randomNumber(1,HUNGER_INCREASE_RATE);
}
/**
 * Random number creator
 * @param min
 * @param max
 * @return
 */
int randomNumber(int min, int max){
    return rand() % max + min;
}
/**
 * Randomly patient creator in to array
 * @param patientsArg
 */
void createPatients(Patient patientsArg[]){
    for (int i = 0; i < PATIENT_NUMBER; ++i) {
        Patient temp;
        temp.patientId = i;
        temp.hungerScore = randomNumber(1,MAX_HUNGER_RESTROOM_SCORE);
        temp.toiletScore = randomNumber(1,MAX_HUNGER_RESTROOM_SCORE);
        temp.disease = randomNumber(0,2);
        patientsArg[i] = temp;
    }
    sem_init(&registration_sem, 0, REGISTRATION_SIZE);
    sem_init(&restroom_sem,0,RESTROOM_SIZE);
    sem_init(&cafe_sem,0,CAFE_NUMBER);
    sem_init(&generalPractitioner_sem,0,GP_NUMBER);
    sem_init(&surgery_sem,0,OR_NUMBER);
    sem_init(&pharmacy_sem,0,PHARMACY_NUMBER);
    sem_init(&bloodLab_sem,0,BLOOD_LAB_NUMBER);
    for (int i = 0; i < PATIENT_NUMBER; ++i) {
        if (pthread_create(&patientsArg[i].pthreadId, NULL,&registration, (Patient*)&patientsArg[i]) != 0) {
            perror("error\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < PATIENT_NUMBER; ++i) {
        if (pthread_join(patientsArg[i].pthreadId, NULL) != 0) {
            perror("error2\n");
            exit(EXIT_FAILURE);
        }
    }
    sem_destroy(&bloodLab_sem);
    sem_destroy(&pharmacy_sem);
    sem_destroy(&surgery_sem);
    sem_destroy(&restroom_sem);
    sem_destroy(&cafe_sem);
    sem_destroy(&registration_sem);
    sem_destroy(&generalPractitioner_sem);
}