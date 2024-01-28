#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <SDL_events.h>

#include <boost/signals2.hpp>

namespace sdl {

class SceneTree;

/**
* Base class for all scene objects.
*/

class Node {

typedef std::unordered_set<Node*> children_t;

public:
	enum ProcessMode {
		PROCESS_MODE_INHERIT /* Inherits the process mode from the node's parent. */,
		PROCESS_MODE_PAUSABLE /* Stops processing when the tree is paused (process when unpaused).*/,
		PROCESS_MODE_WHEN_PAUSED /* Only process when the tree is paused (don't process when unpaused).*/,
		PROCESS_MODE_ALWAYS /* Always process regardless of if the tree is paused or not.*/,
		PROCESS_MODE_DISABLED /* Don't process regardless when the tree is paused or not.*/
	};

	enum {
		NOTIFICATION_READY = 1000 /* Notification received when the node is ready. @see _ready.*/,
		NOTIFICATION_EVENT /* Notification received when an event has been process. @see _event.*/,
		NOTIFICATION_RENDER /* Notification received each render step. @see _render.*/,
		NOTIFICATION_LOOP /* Notification received each loop step. @see _loop.*/,
		NOTIFICATION_PHYSICS_PROCESS /* Notification received each physics step. @see _physics_process.*/,
		NOTIFICATION_ENTER_TREE /* Notification received when the node enters the tree*/,
		NOTIFICATION_EXIT_TREE /* Notification received when the node is removed from the tree.*/,
		NOTIFICATION_PAUSED /* Notification received when the processing of this node has been paused.*/,
		NOTIFICATION_UNPAUSED /* Notification received when the processing of this node has been unpaused.*/,
		NOTIFICATION_PARENTED /* Notification received when the parent is set. Note: This doesn't mean the parent is valid.*/,
		NOTIFICATION_DISABLED /* Notification received when the node is disabled. @see PROCESS_MODE_DISABLED.*/,
		NOTIFICATION_ENABLED /* Notification received when the node is enabled after being disabled. @see PROCESS_MODE_DISABLED.*/,
		NOTIFICATION_CLOSE_REQUEST /* Notification received when the window is about to be closed.*/,
		NOTIFICATION_PREDELETE /* Notification received before the Node is deleted with free.*/
	};

private:
	children_t children;
	SceneTree *tree = nullptr;
	Node *parent;
	std::string name;
	bool is_ready, is_deletion_queued;

protected:
	/**
	* Called when the node receives a notification by notification.
	* \param what The notification. Can be compared to any integer constant.
	*/
	virtual void _notification(const int what);

	/**
	* Called each time an "event" happens.
	* This can be keyboard input, mouse movement, window size changing, etc.
	* @note vents are instantly processed from when they happen.
	*/
	virtual void _event(const SDL_Event *event);

	/**
	* Called in the render step of the tree.
	* This usually happens every 1/frame_rate seconds
	* but can take longer if the system cannot catch up.
	* \param delta The time from between the previous frame and the current frame. Note: This value is very close to 0 on the first frame.
	*/
	virtual void _render(double delta);

	/**
	* Called in the loop step of the tree.
	* This usually happens every 1/fixed_frame_rate seconds
	* but can take longer if the system cannot catch up.
	* \param delta The time from between the previous frame and the current frame. Note: This value is very close to 0 on the first frame.
	*/
	virtual void _loop(double delta);

	/**
	* Called in each physics step of the tree.
	* This usually happens every 1/physics_frame_rate seconds
	* but can take longer if the system cannot catch up.
	* \param delta The time from between the previous frame and the current frame. Note: This value is very close to 0 on the first frame.
	*/
	virtual void _physics_process(const double delta);

	/**
	* Called when the node is "ready".
	* A node is "ready" when the set_tree has been called with a valid SceneTree.
	* This only gets called once and not on any subsequent set_tree.
	*/
	virtual void _ready();

public:
	Node();

	/**
	* Destroys the node and deletes all its child nodes.
	* @note the children should be heap allocated.
	*/
	virtual ~Node();

	/**
	* Emitted when the node is ready. Called after _ready callback and follows the same rules.
	*/
	boost::signals2::signal<void()> ready;

	/**
	* Emitted when the node is renamed.
	*/
	boost::signals2::signal<void(const std::string &name)> renamed;

	/**
	* Emitted when the node enters the tree.
	* This signal is emitted before the related NOTIFICATION_ENTER_TREE notification.
	*/
	boost::signals2::signal<void()> tree_entering;

	/**
	* Emitted when the node is still active but about to exit the tree. This is the right place for de-initialization (or a "destructor", if you will).
	* This signal is emitted before the related NOTIFICATION_EXIT_TREE notification
	*/
	boost::signals2::signal<void()> tree_exiting;

	/**
	* Gets the global event from the tree or nullptr if the tree is not set.
	*/
	SDL_Event *get_event() const;

	/**
	* @returns the time between the previous render step and the current render step.
	* @note This value is very close to 0 on the first frame.
	*/
	double get_delta_time() const;

	/**
	* @returns the time between the previous loop step and the c urrent loop step.
	* @note This value is very close to 0 on the first frame.
	*/
	double get_loop_delta_time() const;

	/**
	* @returns the time between the previous physics step and the current physics step.
	* @note this value is very close to 0 on the first frame.
	* @note always returns the 0 when physics is not enabled.
	*/
	double get_physics_delta_time() const;

	/**
	* Sends the given @param what notification to the node, calling _notification with @param what.
	*/
	void notification(const int what);

	/**
	* Calls notification with @param what to itself and all children.
	*/
	void propagate_notification(const int what);

	/**
	* Queues a node for deletion at the end of the current frame.
	* When deleted, all references to the node will become invalid.
	* It is safe to call queue_free multiple times per frame on a node, and to free a node that is currently queued for deletion. Use is_queued_for_deletion to check whether an node will be deleted at the end of the frame.
	* The node will only be freed after all other deferred calls are finished.
	*/
	void queue_free();

	/**
	* @returns true if the node is queued for deletion.
	*/
	bool is_queued_for_deletion() const;

	/**
	* @returns the tree this node belongs to or nullptr if the tree hasn't been set.
	*/
	SceneTree *get_tree() const;

	/**
	* Sets the tree for this node,
	* calling notification with NOTIFICATION_ENTERED_TREE and NOTIFICATION_READY if the tree hasn't been set.
	*/
	void set_tree(SceneTree *new_tree);

	/**
	* @returns true if the tree property is set to a valid Tree, otherwise false.
	*/
	bool is_inside_tree() const;

	/**
	* Sets the name of the node.
	*/
	void set_name(const std::string &new_name);

	/**
	* @returns the name of the node.
	*/
	const std::string &get_name() const;

	/**
	* Adds the @param child as a child to this node. Reparenting the @param child if it already has a parent.
	* @note the @param child MUST be allocated on the heap and not be deleted without calling remove_node.
	*/
	void add_child(Node *child);

	/**
	* Removes the @param child as a child from this node and sets the parent and tree property to nullptr.
	* Does nothing if the child does not exist or is not a parent of this node.
	*/
	void remove_child(Node *child);

	/**
	* @returns all children.
	*/
	const children_t &get_children() const;

	/**
	* @returns the parent of this node.
	*/
	Node *get_parent() const;

	/**
	* Calls remove_node on all the children of this node.
	*/
	void remove_children();
};

}
