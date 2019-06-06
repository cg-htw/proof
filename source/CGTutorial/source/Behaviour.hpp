class Behaviour {
public:
    Behaviour();   // Constructor.
    virtual void update() = 0;
    virtual void addModel( Model model );
    virtual void removeModel( Model model );
};
