package hw1;

import java.util.ArrayList;

public class Document implements DocElement {
    private String title;
    private ArrayList<DocElement> elements;

    public Document(String title){
        this.title = title;
        this.elements = new ArrayList<DocElement>();
    }

    public ArrayList<DocElement> getElements() {
        return elements;
    }

    public void setElements(ArrayList<DocElement> arr) {
        this.elements = new ArrayList<DocElement>();
        for(int i=0; i<arr.size(); i++){
            this.elements.add(arr.get(i));
        }
    }

    public void add(DocElement de){
        elements.add(de);
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    public <T> T accept(TextVisitor<T> visitor) {
        return visitor.visit(this);
    }
}
