package hw2;

public class StudyGroup {
    String name;
    Lab lab;

    // Constructor
    StudyGroup(String name, Lab lab){
        this.name = name;
        this.lab = lab;
    }

    // Getter functions
    public String getName() {
        return name;
    }

    public Lab getLab() {
        return lab;
    }

    // Calls addStudent method of lab class
    public void startStudyingWith(){
        lab.addStudent(this.name);
    }

    // Calls removeStudent method of lab class
    public void stopStudyingWith(){
        lab.removeStudent();
    }
}
