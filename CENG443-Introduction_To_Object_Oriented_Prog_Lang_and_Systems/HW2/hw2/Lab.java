package hw2;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Lab {
    private String name;
    private int capacity;

    // studentCount variable is needed to check whether the lab is occupied or not
    private int studentCount;

    // currentStudyGroup variable is needed to control the entrance of the students to the lab if they are only from same studyGroup
    private String currentStudyGroup;

    private Lock lock = new ReentrantLock();

    // canEnter and canLeave Condition Variables to synchronize
    // a student canEnter if certain conditions are met
    // a student canLeave if certain conditions are met
    private Condition canEnter = lock.newCondition();
    private Condition canLeave = lock.newCondition();

    // Constructor
    // Initially, there is no student and no student group
    public Lab(String name, int capacity){
        this.name = name;
        this.capacity = capacity;
        this.studentCount = 0;
        currentStudyGroup = null;
    }

    // Getter methods
    public String getName() {
        return name;
    }

    public int getCapacity() {
        return capacity;
    }

    // Method for entrance of a student to the lab
    // It takes studyGroupName as the only parameter to check the student is in the same group with that of wanted lab
    public void addStudent(String studyGroupName){
        lock.lock();

        try{
            // If a studyGroup is in the lab and it is not same with the studyGroup of the student want to enter, he/she must wait
            // Or if there is no space to enter, the student must wait
            while((currentStudyGroup != null && !currentStudyGroup.equals(studyGroupName)) || (studentCount == capacity)){
                canEnter.await();
            }

            // From now, the student has entered, and so we increment the studentCount by 1
            studentCount = studentCount + 1;

            // If the lab is empty before the student entered, since he/she is the first student using the lab,
            // currentStudyGroup must be assigned to his/her group
            if(studentCount == 1){
                currentStudyGroup = studyGroupName;
            }

            // Since the lab is not empty now, students can leave
            canLeave.signal();
        }

        catch (InterruptedException e) {
            e.printStackTrace();
        }

        // Unlock to allow other threads
        finally {
            lock.unlock();
        }
    }

    public void removeStudent(){
        lock.lock();
        try{
            // If there is no student, leaving cannot be happened, and so it must wait
            while(studentCount == 0){
                canLeave.await();
            }

            // From now, a student has left the lab, and so studentCount must be decremented
            studentCount = studentCount - 1;

            // If he/she was the last student using the lab, the lab is not specific for a studyGroup from now on
            if(studentCount == 0){
                currentStudyGroup = null;
            }

            // New students can enter, so notify them
            canEnter.signal();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        finally {
            // Unlock to allow other threads
            lock.unlock();
        }
    }
}
