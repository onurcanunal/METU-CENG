package hw1;

public interface Rule {
    default void clear(){
        getPremise().accept(new ClearVarsVisitor());
        getEntails().accept(new ClearVarsVisitor());
    }

    default boolean apply(MathExpression me){
        clear();
        if(getPremise().match(me)){
            return true;
        }
        else{
            clear();
            return false;
        }
    }

    MathExpression getPremise();

    MathExpression getEntails();

    default MathExpression entails(MathExpression me){
        apply(me);
        return getEntails();
    }
}
