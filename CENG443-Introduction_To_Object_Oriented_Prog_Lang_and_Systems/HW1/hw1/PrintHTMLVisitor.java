package hw1;

public class PrintHTMLVisitor implements TextVisitor<String> {
    public String visit(Document document) {
        String elementsText = "";
        for(int i=0; i<document.getElements().size(); i++){
            elementsText = elementsText + document.getElements().get(i).accept(new PrintHTMLVisitor());
        }
        return "<html>" +
                "<head>" +
                "<title>"+
                document.getTitle() +
                "</title>" +
                "</head>" +
                "<body>"+
                elementsText+
                "</body>" +
                "</html>";

    }

    public String visit(EquationText equationText) {
        return "<math>" + equationText.getInnerMath().accept(new PrintMathMLVisitor()) + "</math>";
    }

    public String visit(Paragraph paragraph) {
        return "<p>" + paragraph.getText() + "</p>";
    }
}
