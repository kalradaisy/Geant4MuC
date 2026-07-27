#include "ActionInitialization.hh"
#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"


void ActionInitialization::BuildForMaster() const{
    /* Master thread handles the higher-level processes, such as the geometry
    and physics setup. So, let's hand it the run action so it knows what the
    worker threads are working with. The master thread won't actually do the
    work in RunAction, but it will be responsible for merging all the work that
    the worker threads do in RunAction, so it needs context.*/
    SetUserAction(new RunAction());
}

/* The function below is what the worker threads actually call to do their work.
Worker threads have a very limited scope. They know just what they need to know.
In this case, they know what to do when the RunAction fires, and they carry out
that action. When they're done, the master thread will collect their work.*/
void ActionInitialization::Build() const
{
    /* Then they have what the run action is. Here's what's gonna happen when
    that gun goes off. */
    auto runAction = new RunAction();
    SetUserAction(runAction);
    /* Primary generator, mandatory for the workers. Sets the groundwork.
    Here is a gun, get ready, because we're about to fire it.*/
    SetUserAction(new PrimaryGenerator(runAction));
    /* And here's what you'll do to keep track of its projectile. I imagine this
    is akin to the notebook in which the worker will write down the information
    that they see from the interaction.*/
    auto eventAction = new EventAction();
    SetUserAction(eventAction);
    /* And here's how you'll keep going, step after step.*/
    SetUserAction(new SteppingAction(eventAction, runAction));
    /* A place to record the information for the tracks and whatnot*/
    SetUserAction(new TrackingAction());
}
