#pragma once

#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <SDL.h>
#include <deque>

#include "vec.h"
#include "bounds.h"

////////////////////////////////////////////////////////////
///	The Stroke class allow to draw real-time animated strokes.
///
/// \example
/// Stroke my_stroke;
/// // A stroke need at least 3 joints to be displayed.
/// my_stroke.AddJoint(40, 40);
/// my_stroke.AddJoint(200, 50);
/// // Set the precompile parameter to true if you want to
/// // precompile your stroke while loading your application.
/// my_stroke.AddJoint(230, 400, true);
///
/// while(true)
/// {
///		App.Clear();
///		App.Draw(my_stroke);
///		App.Display();
/// }
/// \endexample
///
/// \example
/// Stroke my_stroke;
/// // We add some speed to the joints to make the stroke grow upward.
/// my_stroke.SetJointLinearSpeed(0, -200);
///
/// while(true)
/// {
///		// Precompiling is useless because the stroke will change each frame.
///		my_stroke.AddJoint(400, 300);
///
///		// We need to compute the joints to delete
///		// useless joints and move existing ones.
///		my_stroke.Calculate(App.GetFrameTime());
///
///		App.Clear();
///		App.Draw(my_stroke);
///		App.Display();
/// }
/// \endexample
////////////////////////////////////////////////////////////
class Stroke
{
	public:
		enum class SweepStyle
		{
			Soft = 0,
			Softer,
			Sharp
		};

	private:
		////////////////////////////////////////////////////////////
		/// A single joint of the stroke
		////////////////////////////////////////////////////////////
		struct Joint
		{
			Joint();
			vec	position;
			vec	offset;
			float			birth_time;
			float			thickness;
			float			angle;
			vec	upper_point;
			vec	lower_point;
			SDL_Color		inner_color;
			SDL_Color		outer_color;
			float			thickness_scale;
		};

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		// Bones
		std::deque<Joint>			joints;				///< The joints of the stroke
		float						time;				///< The lifetime elapsed, used with joints lifetime

		// Body structure
		unsigned int				max_joints;			///< Maximum joints the stroke can have
		float						max_length;			///< Maximum length the stroke can have
		float						min_segment_length; ///< Minimum length between each segments
		float						joint_lifetime;		///< Maximum time the joints can stay
		vec							joint_linear_speed; ///< Linear speed the joints move

		// Stroke option
		float						thickness;			///< Global thickness
		vec							random_thickness;	///< Random thickness applyed when adding joint
		float						start_thickness;	///< Thickness at the start of the stroke
		float						end_thickness;		///< Thickness at the end of the stroke
		float						stroke_offset;		///< Position of the separation between start and end
		float						color_offset;		///< Position of the separation between start and end for color interpolation
		float						start_sweep;		///< Amplitude of the sweep effect at the start
		float						end_sweep;			///< Amplitude of the sweep effect at the end
		SweepStyle					start_sweep_style;	///< Effect used at the start of the stroke
		SweepStyle					end_sweep_style;	///< Effect used at the end of the stroke
		float						stippling;			///< Amplitude of the stippling effect
		float						shaking;			///< Amplitude of the shaking effect
		bool						loop;				///< Loop the stroke by joining first and last joint

		// Appearance
		SDL_Color					start_inner_color;	///< Inner color at the start of the stroke
		SDL_Color					start_outer_color;	///< Outer color at the start of the stroke
		SDL_Color					end_inner_color;	///< Inner color at the end of the stroke
		SDL_Color					end_outer_color;	///< Outer color at the end of the stroke

		// Length
		bool						is_length_compiled;	///< Flag for lengh computation
		float						length;				///< Total length of the stroke

		// State
		bool						is_compiled;		///< Flag for stroke computation

		////////////////////////////////////////////////////////////
		/// Draw the object
		///
		/// DrawSolid draw the stroke with a solid style, meaning
		/// the stroke is composed with gradient polygons.
		////////////////////////////////////////////////////////////
		void DrawSolid() const;
		void DrawSolidExcluding(const Stroke& other) const;

		////////////////////////////////////////////////////////////
		/// Draw a bloc in the DrawSolid function
		///
		/// A 'bloc' is one of the polygons drawn to form the final stroke.
		///
		/// \param current_joint Identifier of the current joint
		/// \param previous_joint Identifier of the previous joint
		////////////////////////////////////////////////////////////
		void DrawSolidBloc(unsigned int current_joint, unsigned int previous_joint) const;
		void DrawSolidBlocExcluding(const Stroke& other, unsigned int current_joint, unsigned int previous_joint) const;

		////////////////////////////////////////////////////////////
		/// Compile the stroke
		///
		/// This function precomputes all the internal parameters
		/// needed to properly render the stroke (external points, thickness).
		////////////////////////////////////////////////////////////
		bool Compile();

		////////////////////////////////////////////////////////////
		/// Compute the shaking effect
		///
		/// This function precomputes the joints offset due to the
		/// shaking effect.
		/// It is called only if the shaking is not at 0.
		////////////////////////////////////////////////////////////
		void ComputeOffset();

		////////////////////////////////////////////////////////////
		/// Compute the length
		///
		/// This function precomputes the total length of the stroke,
		/// in pixels. It is called only if joints are added, moved or
		/// removed.
		////////////////////////////////////////////////////////////
		void CompileLength();

		////////////////////////////////////////////////////////////
		/// Compute the thickness
		///
		/// This function precomputes the joints thickness regarding
		/// the thickness parameters.
		///
		/// \param alpha	The alpha value of the current joint,
		///					meaning the length of the joint divised by
		///					the total length. (Its position on the stroke.)
		/// \param offset	The offset of the sweep.
		/// \param t		The base joint thickness multiplyed by
		///					the global thickness.
		////////////////////////////////////////////////////////////
		float ComputeThickness(float alpha, float offset, float t);

		////////////////////////////////////////////////////////////
		/// Compute the sweep
		///
		/// This function precomputes the stroke sweep effect for
		/// the current joint, regarding the desired sweep style.
		///
		/// \param sweep_style	The wanted sweep style to apply
		/// \param a			The current thickness
		///
		/// \return The new thickness regarding the sweep.
		////////////////////////////////////////////////////////////
		float ApplySweep(SweepStyle sweep_style, float a);

		////////////////////////////////////////////////////////////
		/// Interpolate between two colors
		///
		/// \param color1	First color
		/// \param color2	Second color
		/// \param alpha	The alpha value, in the range [0, 1]
		///
		/// \return	The interpolated color
		////////////////////////////////////////////////////////////
		inline SDL_Color ColorInterpolation(const SDL_Color & color1, const SDL_Color & color2, float alpha);

	public:
		////////////////////////////////////////////////////////////
		/// Default constructor
		///
		/// Creates an empty stroke (no joints).
		////////////////////////////////////////////////////////////
		Stroke();

		////////////////////////////////////////////////////////////
		/// Render the stroke.
		///
		/// The function will compile the stroke and its length if needed.
		////////////////////////////////////////////////////////////
		void Draw() const;
		void DrawExcluding(const Stroke& other) const;

		////////////////////////////////////////////////////////////
		/// Get the linear speed applied to the points
		///
		/// \return	Linear speed, in pixels/sec
		///
		/// \see SetJointLinearSpeed
		////////////////////////////////////////////////////////////
		const vec& GetJointLinearSpeed() const {
			return joint_linear_speed;
		}

		////////////////////////////////////////////////////////////
		/// Get the maximum joints the stroke can contain
		///
		/// \return	Maximum joints of the stroke
		///
		/// \see SetMaxJoints
		////////////////////////////////////////////////////////////
		unsigned int GetMaxJoints() const {
			return max_joints;
		}

		////////////////////////////////////////////////////////////
		/// Get the current joints the stroke has
		///
		/// \return	Current joints of the stroke
		///
		/// \see SetMaxJoints
		////////////////////////////////////////////////////////////
		unsigned int GetJointsCount() const {
			return joints.size();
		}

		////////////////////////////////////////////////////////////
		/// Get the maximum length the stroke can have
		///
		/// \return	Maximum length of the stroke, in pixels
		///
		/// \see SetMaxLength
		////////////////////////////////////////////////////////////
		float GetMaxLength() const {
			return max_length;
		}

		////////////////////////////////////////////////////////////
		/// Get the minimum length between each joint
		///
		/// \return	Minimum length, in pixels
		///
		/// \see SetMinSegmentLength
		////////////////////////////////////////////////////////////
		float GetMinSegmentLength() const {
			return min_segment_length;
		}

		////////////////////////////////////////////////////////////
		/// Get the lifetime of each new joint
		///
		/// \return	Lifetime, in seconds
		///
		/// \see SetJointLifeTime
		////////////////////////////////////////////////////////////
		float GetJointLifetime() const {
			return joint_lifetime;
		}

		////////////////////////////////////////////////////////////
		/// Get the global thickness of the stroke
		///
		/// \return	Global thickness, un pixels
		///
		/// \see SetThickness
		////////////////////////////////////////////////////////////
		float GetThickness() const {
			return thickness;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the maximum thickness that can
		/// be randomly generated
		///
		/// \return	Maximum thickness
		///
		/// \see SetMaxRandomThickness
		////////////////////////////////////////////////////////////
		float GetMaxRandomThickness() const {
			return random_thickness.y;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the minimum thickness that can
		/// be randomly generated
		///
		/// \return	Minimum thickness
		///
		/// \see SetMinRandomThickness
		////////////////////////////////////////////////////////////
		float GetMinRandomThickness() const {
			return random_thickness.x;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the thickness at the start of the stroke.
		///
		/// \return	Start thickness, in range [0, 1]
		///
		/// \see SetStartThickness
		////////////////////////////////////////////////////////////
		float GetStartThickness() const {
			return start_thickness;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the thickness at the end of the stroke.
		///
		/// \return	End thickness, in range [0,1]
		///
		/// \see SetEndThickness
		////////////////////////////////////////////////////////////
		float GetEndThickness() const{
			return end_thickness;
		}

		////////////////////////////////////////////////////////////
		/// Get the offset separating the start from the end of the stroke.
		///
		/// \return	Offset, in range [-1, 1]
		///
		/// \see SetOffset
		////////////////////////////////////////////////////////////
		float GetOffset() const {
			return stroke_offset;
		}

		////////////////////////////////////////////////////////////
		/// Get the offset separating the start from the end of the stroke for color interpolation.
		///
		/// \return	The offset
		///
		/// \see SetColorOffset
		////////////////////////////////////////////////////////////
		float GetColorOffset() const {
			return color_offset;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the sweep at the start of the stroke.
		///
		/// \return	Start sweep, in range [0, 1]
		///
		/// \see SetStartSweep
		////////////////////////////////////////////////////////////
		float GetStartSweep() const {
			return start_sweep;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the sweep at the end of the stroke.
		///
		/// \return	End sweep, in range [0, 1]
		///
		/// \see SetEndSweep
		////////////////////////////////////////////////////////////
		float GetEndSweep() const {
			return end_sweep;
		}

		////////////////////////////////////////////////////////////
		/// Get the style of the sweep at the start of the stroke.
		///
		/// \return	Sweep style
		///
		/// \see SetStartSweepStyle
		////////////////////////////////////////////////////////////
		SweepStyle GetStartSweepStyle() const {
			return start_sweep_style;
		}

		////////////////////////////////////////////////////////////
		/// Get the style of the sweep at the end of the stroke.
		///
		/// \return	Sweep style
		///
		/// \see SetEndSweepStyle
		////////////////////////////////////////////////////////////
		SweepStyle GetEndSweepStyle() const {
			return end_sweep_style;
		}

		////////////////////////////////////////////////////////////
		/// Get the quotient of the stippling effect
		///
		/// \return	Stippling, in range [0, 1]
		///
		/// \see SetStippling
		////////////////////////////////////////////////////////////
		float GetStippling() const {
			return stippling;
		}

		////////////////////////////////////////////////////////////
		/// Get the amplitude of the shaking effect
		///
		/// \return	Shaking amplitude, in pixels
		///
		/// \see SetShaking
		////////////////////////////////////////////////////////////
		float GetShaking() const {
			return shaking;
		}

		////////////////////////////////////////////////////////////
		/// Get the looping flag
		///
		/// \return	True if the stroke loop
		///
		/// \see SetLoop
		////////////////////////////////////////////////////////////
		bool IsLooping() const {
			return loop; 
		}

		////////////////////////////////////////////////////////////
		/// Get the inner color at the start of the stroke
		///
		/// \return	Start inner color
		///
		/// \see SetStartInnerColor, SetInnerColor
		////////////////////////////////////////////////////////////
		const SDL_Color& GetStartInnerColor() const {
			return start_inner_color;
		}

		////////////////////////////////////////////////////////////
		/// Get the outer color at the start of the stroke
		///
		/// \return	Start outer color
		///
		/// \see SetStartOuterColor, SetOuterColor
		////////////////////////////////////////////////////////////
		const SDL_Color& GetStartOuterColor() const {
			return start_outer_color;
		}

		////////////////////////////////////////////////////////////
		/// Get the inner color at the end of the stroke
		///
		/// \return	End inner color
		///
		/// \see SetEndInnerColor, SetInnerColor
		////////////////////////////////////////////////////////////
		const SDL_Color& GetEndInnerColor() const {
			return end_inner_color;
		}

		////////////////////////////////////////////////////////////
		/// Get the outer color at the end of the stroke
		///
		/// \return	End outer color
		///
		/// \see SetEndOuterColor, SetOuterColor
		////////////////////////////////////////////////////////////
		const SDL_Color& GetEndOuterColor() const {
			return end_outer_color;
		}

		////////////////////////////////////////////////////////////
		/// Get total length of the stroke
		///
		/// \return Total length
		////////////////////////////////////////////////////////////
		float GetLength();

		////////////////////////////////////////////////////////////
		/// Get the bounding box of the stroke
		///
		/// The bounding box is a rectangle incorporating all the points
		/// created to draw the stroke. It is usefull for collision test.
		/// For example, a stroke from wich the bounding box is out of the
		/// current view isn't visible on the screen, and thus it don't need
		/// to be drawn.
		///
		/// \return	Rectangle defining the bounding box of the stroke
		////////////////////////////////////////////////////////////
		BoxBounds ComputeBoundingBox();

		////////////////////////////////////////////////////////////
		/// Indicate if the stroke will be appearent if we draw it
		///
		/// This function indicate if the stroke have the minimum requierements
		/// to be visible on screen. The requierements are:
		/// - at least 3 joints
		/// - a thickness different from 0
		///
		/// \return	True if the stroke has enough joints
		////////////////////////////////////////////////////////////
		bool IsDrawable() const;

		// Modificateurs de l'objet
		////////////////////////////////////////////////////////////
		/// Set the joint's linear speed
		///
		/// The linear speed allow the joints to move themselves. It requier
		/// that the stroke is made dynamic and keep its joints free from
		/// custom data, meaning the stroke isn't cleared at each frame. The
		/// speed is expressed in pixel per second on each axis.
		///
		/// Default value : 0 for each Axis
		///
		///	\param x Horizontal velocity
		/// \param y Vertical velocity
		///
		/// \see GetJointLinearSpeed
		////////////////////////////////////////////////////////////
		void SetJointLinearSpeed(float x, float y);

		////////////////////////////////////////////////////////////
		/// Set the joint's linear speed
		///
		/// The linear speed allow the joints to move themselves. It requier
		/// that the stroke is made dynamic and keep its joints free from
		/// custom data, meaning the stroke isn't cleared at each frame. The
		/// speed is expressed in pixel per second on each axis.
		///
		/// Default value : 0 for each Axis
		///
		///	\param linear_speed Linear speed in pixel/sec
		///
		/// \see GetJointLinearSpeed
		////////////////////////////////////////////////////////////
		void SetJointLinearSpeed(const vec & linear_speed);

		////////////////////////////////////////////////////////////
		/// Set the maximum joints the stroke can have
		///
		/// Each stroke is composed of a number of joints. Having a
		/// restriction on their number can avoid having a stroke
		/// infinitely growing. It is a way to constraint the length
		/// of a stroke.
		///
		/// Default value : 0
		///
		/// Warning : if you want to use a dynamic stroke, be sure to
		/// set a maximum value to at least one of the following functions:
		/// - SetMaxJoints
		/// - SetMaxLength
		/// - SetJointLifetime
		///
		///	\param max_joints The maximum joints the stroke can have
		///
		/// \see GetMaxJoints, SetMaxJoints, SetMaxLength, SetJointLifetime
		////////////////////////////////////////////////////////////
		void SetMaxJoints(unsigned int max_joints);

		////////////////////////////////////////////////////////////
		/// Set the maximum length the stroke can have
		///
		/// Stroke don't have limits in length. This function allow
		/// you to set a limit to the length. The length is basically
		/// the sum of the distances between each joints, not the distance
		/// between the first and last joint.
		/// If the length of the stroke became too long when adding new joints
		/// with \a AddJoint function, the \ Calculate function will remove
		/// the last joints of the list until the length get back smaller than
		/// the maximum length.
		/// A maximum length of 0 let the stroke grow infinitely.
		///
		/// Warning : if you want to use a dynamic stroke, be sure to
		/// set a maximum value to at least one of the following functions:
		/// - SetMaxJoints
		/// - SetMaxLength
		/// - SetJointLifetime
		///
		/// Default value : 0
		///
		///	\param max_length The maximum length the stroke can have, in pixels
		///
		/// \see GetMaxLength, SetMaxJoints, SetMaxLength, SetJointLifetime
		////////////////////////////////////////////////////////////
		void SetMaxLength(float max_length);

		////////////////////////////////////////////////////////////
		/// Set the minimum length between each joint
		///
		/// Having too few joints give to your stroke a sad polygonal look,
		/// but too much can be a threat to your program performances.
		/// The minimum length ensure a new joint placed with the /a AddJoint
		/// function will have a minimum length with the previous one to
		/// be added. If it is too close to the previous joint, the new joint
		/// won't be added. If it is too far, intermediate joints will be added
		/// forming a straight line.
		/// A minimum length of 0 allow ANY new joints to be added.
		///
		/// Default value : 0
		///
		///	\param min_segment_length The minimum length between each joint, in pixels
		///
		/// \see GetMinSegmentLength
		////////////////////////////////////////////////////////////
		void SetMinSegmentLength(float min_segment_length);

		////////////////////////////////////////////////////////////
		/// Set the lifetime of each joints
		///
		/// This function give a maximum lifetime to your joints. Too
		/// old joints will be removed by the \a Calculate function.
		/// It is a cool way to create organic strokes depleting in time.
		/// It is also a way to avoid infinitely growing strokes.
		///
		/// Warning : if you want to use a dynamic stroke, be sure to
		/// set a maximum value to at least one of the following functions:
		/// - SetMaxJoints
		/// - SetMaxLength
		/// - SetJointLifetime
		///
		/// Default value : 0
		///
		///	\param joint_lifetime The lifetime of each joint, in seconds
		///
		/// \see GetJointLifetime
		////////////////////////////////////////////////////////////
		void SetJointLifetime(float joint_lifetime);

		////////////////////////////////////////////////////////////
		/// Set the global thickness of the stroke
		///
		/// The global thickness is the main thickness of the stroke. It
		/// prevail over the overthickness, meaning a stroke with a thickness
		/// of 0 won't be drawn.
		///
		/// Default value : 3 pixels
		///
		///	\param thickness The global thickness, in pixels
		///
		/// \see GetThickness
		////////////////////////////////////////////////////////////
		void SetThickness(float thickness);

		////////////////////////////////////////////////////////////
		/// Set the range of random thickness
		///
		/// Each joint possess a thickness of it's own. Settings a thickness
		/// with this function will give a randomly choosed thickness to each
		/// new joint added with the \a AddJoint function.
		/// The random thickness take multiples of the global thickness.
		/// Example :
		/// \code
		/// mu_stroke.SetThickness(30.f);
		/// my_stroke.SetRandomThickness(0.5f, 1.5f);
		/// \endcode
		/// With this code, each new joint will receive a thickness
		/// between 15 and 45 pixels.
		///
		/// Default value : 1 to min and max
		///
		///	\param min Minimum possible value
		/// \param max Maximim possible value
		///
		/// \see GetMaxRandomThickness, GetMinRandomThickness
		////////////////////////////////////////////////////////////
		void SetRandomThickness(float min, float max);

		////////////////////////////////////////////////////////////
		/// Set the thickness at the start of the stroke
		///
		/// The stroke have a starting and ending thickness. It allow it
		/// to have a smoothed shape, defined by both the start/end thickness,
		/// sweep and sweep style.
		/// A thickness of 0 will make the stroke tapered at the start,
		/// a thickness of 1 will make it squared.
		///
		/// Default value : 1
		///
		///	\param start_thickness Thickness at the start of the stroke, in range [0, 1]
		///
		/// \see GetStartThickness
		////////////////////////////////////////////////////////////
		void SetStartThickness(float start_thickness);

		////////////////////////////////////////////////////////////
		/// Set the thickness at the end of the stroke
		///
		/// The stroke have a starting and ending thickness. It allow it
		/// to have a smoothed shape, defined by both the start/end thickness,
		/// sweep and sweep style.
		/// A thickness of 0 will make the stroke tapered at the end,
		/// a thickness of 1 will make it squared.
		///
		/// Default value : 1
		///
		///	\param end_thickness Thickness at the end of the stroke, in range [0, 1]
		///
		/// \see GetEndThickness
		////////////////////////////////////////////////////////////
		void SetEndThickness(float end_thickness);

		////////////////////////////////////////////////////////////
		/// Set the offset of the stroke
		///
		/// Having a stroke with tapered start and end give it a more
		/// organic look. The offset allow you to move the separation
		/// between each part of the stroke (start and end) along the stroke.
		/// An offset of 1 will move it to the start of the stroke,
		/// an offset of -1 will move it at the end and an offset of
		/// 0 keep it at midway of the stroke.
		/// The offset is useless if neither of the start or end of the stroke
		/// have a thickness lower that 1.
		///
		/// Default value : 0
		///
		///	\param stroke_offset Offset of the stroke, in range [-1, 1]
		///
		/// \see GetOffset
		////////////////////////////////////////////////////////////
		void SetOffset(float stroke_offset);

		////////////////////////////////////////////////////////////
		/// Set the color offset of the stroke
		///
		/// Defines when the color will change between start and end.
		///
		/// Default value : 0
		///
		///	\param color_offset Color offset of the stroke. Higher values 
		///                     make the gradient change more towards the end.
		///
		/// \see GetOffset
		////////////////////////////////////////////////////////////
		void SetColorOffset(float color_offset);

		////////////////////////////////////////////////////////////
		/// Set the sweep of the stroke
		///
		/// The default tapered stroke take a diamond shape. The sweep
		/// allow to give your stroke a more swift and smooth shape.
		/// A sweep of 0 will keep the stroke a diamond shape, while
		/// a sweep of 1 will give it the full sweep shape defined with
		/// the sweep style.
		///
		/// SetSweep function set the sweep for both start and end of the stroke.
		///
		/// Default value : 0
		///
		///	\param sweep Sweep of the stroke, in range [0, 1]
		///
		/// \see GetStartSweep, GetEndSweep, SetStartSweep, SetEndSweep, SetSweepStyle
		////////////////////////////////////////////////////////////
		void SetSweep(float sweep);

		////////////////////////////////////////////////////////////
		/// Set the start sweep of the stroke
		///
		/// The default tapered stroke take a diamond shape. The sweep
		/// allow to give your stroke a more swift and smooth shape.
		/// A sweep of 0 will keep the stroke a diamond shape, while
		/// a sweep of 1 will give it the full sweep shape defined with
		/// the sweep style.
		///
		/// Default value : 0
		///
		///	\param start_sweep Sweep of the start of the stroke, in range [0, 1]
		///
		/// \see GetStartSweep, SetSweepStyle
		////////////////////////////////////////////////////////////
		void SetStartSweep(float start_sweep);

		////////////////////////////////////////////////////////////
		/// Set the end sweep of the stroke
		///
		/// The default tapered stroke take a diamond shape. The sweep
		/// allow to give your stroke a more swift and smooth shape.
		/// A sweep of 0 will keep the stroke a diamond shape, while
		/// a sweep of 1 will give it the full sweep shape defined with
		/// the sweep style.
		///
		/// Default value : 0
		///
		///	\param end_sweep Sweep of the end of the stroke, in range [0, 1]
		///
		/// \see GetEndSweep, SetSweepStyle
		////////////////////////////////////////////////////////////
		void SetEndSweep(float end_sweep);

		////////////////////////////////////////////////////////////
		/// Set the sweep style of the stroke
		///
		/// The sweep style is the shape the tapered stroke will take
		/// with a sweep greater than 0.
		///
		/// The sweep style can be Soft, Softer or Sharp.
		///
		/// SetSweepStyle set the sweep style for both start and end of the stroke.
		///
		/// Default value: Soft
		///
		///	\param sweep_style Sweep style of the stroke
		///
		/// \see SweepStyle, GetStartSweepStyle, GetEndSweepStyle
		////////////////////////////////////////////////////////////
		void SetSweepStyle(SweepStyle sweep_style);

		////////////////////////////////////////////////////////////
		/// Set the sweep style at the start of the stroke
		///
		/// The sweep style is the shape the tapered stroke will take
		/// with a sweep greater than 0.
		///
		/// The sweep style can be Soft, Softer or Sharp.
		///
		/// Default value: Soft
		///
		///	\param start_sweep_style Sweep style at the start of the stroke
		///
		/// \see SweepStyle, GetStartSweepStyle
		////////////////////////////////////////////////////////////
		void SetStartSweepStyle(SweepStyle start_sweep_style);

		////////////////////////////////////////////////////////////
		/// Set the sweep style at the end of the stroke
		///
		/// The sweep style is the shape the tapered stroke will take
		/// with a sweep greater than 0.
		///
		/// The sweep style can be Soft, Softer or Sharp.
		///
		/// Default value : Soft
		///
		///	\param end_sweep_style Sweep style at the end of the stroke
		///
		/// \see SweepStyle, GetEndSweepStyle
		////////////////////////////////////////////////////////////
		void SetEndSweepStyle(SweepStyle end_sweep_style);

		////////////////////////////////////////////////////////////
		/// Set the stippling of the stroke
		///
		/// This function allow you to set a stippling quotient for
		/// your stroke. The stippling will give a dotted look to your
		/// stroke.
		///
		/// A stippling of 0 disable the stippling effect while
		/// a stippling of 1 will make the stroke non-displayable.
		///
		/// Default value : 0
		///
		///	\param stippling Stippling of the stroke, in range [0, 1]
		///
		/// \see GetStippling
		////////////////////////////////////////////////////////////
		void SetStippling(float stippling);

		////////////////////////////////////////////////////////////
		/// Set the shaking amplitude of the stroke
		///
		/// The shaking is a simple distortion of the stroke. With
		/// static stroke, the shaking will give a distorded look at
		/// your stroke, with dynamic stroke, it will make the stroke
		/// shake over the time.
		///
		/// Default value : 0
		///
		///	\param shaking Shaking amplitude of the stroke, in pixels
		///
		/// \see GetShaking
		////////////////////////////////////////////////////////////
		void SetShaking(float shaking);

		////////////////////////////////////////////////////////////
		/// Set the shaking amplitude of the stroke
		///
		/// Looping the stroke can be usefull to draw closed path like
		/// circles or squares. Looping the stroke join the first and last
		/// joint, computing their angles accordingly.
		///
		/// Default value : false
		///
		///	\param loop Set to true if you need to make the stroke looping
		///
		/// \see GetShaking
		////////////////////////////////////////////////////////////
		void SetLoop(bool loop);

		////////////////////////////////////////////////////////////
		/// Set the inner color of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// SetInnerColor set the inner color for both start and end of
		/// the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param inner_color Inner color of the stroke
		///
		/// \see GetStartInnerColor, GetEndInnerColor
		////////////////////////////////////////////////////////////
		void SetInnerColor(const SDL_Color & inner_color);

		////////////////////////////////////////////////////////////
		/// Set the outer color of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// SetOuterColor set the outer color for both start and end of
		/// the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param outer_color Outer color of the stroke
		///
		/// \see GetStartOuterColor, GetEndOuterColor
		////////////////////////////////////////////////////////////
		void SetOuterColor(const SDL_Color & outer_color);

		////////////////////////////////////////////////////////////
		/// Set the inner color at the start of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param start_inner_color Inner color at the start of the stroke
		///
		/// \see GetStartInnerColor
		////////////////////////////////////////////////////////////
		void SetStartInnerColor(const SDL_Color & start_inner_color);

		////////////////////////////////////////////////////////////
		/// Set the outer color at the start of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param start_outer_color Outer color at the start of the stroke
		///
		/// \see GetStartOuterColor
		////////////////////////////////////////////////////////////
		void SetStartOuterColor(const SDL_Color & start_outer_color);

		////////////////////////////////////////////////////////////
		/// Set the inner color at the end of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param end_inner_color Outer color at the end of the stroke
		///
		/// \see GetStartInnerColor
		////////////////////////////////////////////////////////////
		void SetEndInnerColor(const SDL_Color & end_inner_color);

		////////////////////////////////////////////////////////////
		/// Set the outer color at the end of the stroke
		///
		/// The stroke allow to use different color for both in the stroke
		/// and out the stroke. It allow to create gradiant between the center
		/// and extremities of the stroke.
		///
		/// For both inner and outer color, you can also set the color at
		/// the start and the end of the stroke.
		///
		/// Default value : SDL_Color(255, 255, 255)
		///
		///	\param end_outer_color Outer color at the end of the stroke
		///
		/// \see GetEndOuterColor
		////////////////////////////////////////////////////////////
		void SetEndOuterColor(const SDL_Color & end_outer_color);

		////////////////////////////////////////////////////////////
		/// Clear the joint array
		///
		/// Remove all the joints from the stroke.
		////////////////////////////////////////////////////////////
		void Clear();

		////////////////////////////////////////////////////////////
		/// Add a joint to the stroke
		///
		/// Add a joint to the stroke, giving its position and if
		/// a precompilation of the stroke is wanted.
		///
		/// \param x Position of the joint on X Axis (relative position)
		/// \param y Position of the joint on Y Axis (relative position)
		/// \param precompile Set to true if a precompilation is needed
		///
		/// \return True if the joint has been successfully added
		////////////////////////////////////////////////////////////
		bool AddJoint(float x, float y, bool precompile = false);

		////////////////////////////////////////////////////////////
		/// Add a joint to the stroke
		///
		/// Add a joint to the stroke, giving its position and if
		/// a precompilation of the stroke is wanted.
		///
		/// \param position Position of the joint (relative position)
		/// \param precompile Set to true if a precompilation is needed
		///
		/// \return True if the joint has been successfully added
		////////////////////////////////////////////////////////////
		bool AddJoint(const vec & position, bool precompile = false);

		////////////////////////////////////////////////////////////
		/// Add a joint to the stroke
		///
		/// Add a joint to the stroke, giving its position and if
		/// a precompilation of the stroke is wanted.
		///
		/// With the thickness scale, the randomness given by
		/// the random Thickness parameter will be ignored.
		///
		/// \param x Position of the joint on X Axis (relative position)
		/// \param y Position of the joint on Y Axis (relative position)
		/// \param thickness_scale Thickness of the joint
		/// \param precompile Set to true if a precompilation is needed
		///
		/// \return True if the joint has been successfully added
		////////////////////////////////////////////////////////////
		bool AddJoint(float x, float y, float thickness_scale, bool precompile = false);

		////////////////////////////////////////////////////////////
		/// Add a joint to the stroke
		///
		/// Add a joint to the stroke, giving its position and if
		/// a precompilation of the stroke is wanted.
		///
		/// With the thickness scale, the randomness given by
		/// the random Thickness parameter will be ignored.
		///
		/// \param position Position of the joint (relative position)
		/// \param thickness_scale Thickness of the joint
		/// \param precompile Set to true if a precompilation is needed
		///
		/// \return True if the joint has been successfully added
		////////////////////////////////////////////////////////////
		bool AddJoint(const vec & position, float thickness_scale, bool precompile = false);

		////////////////////////////////////////////////////////////
		/// Add a joint to the stroke
		///
		/// Calculate must be called if you want to make your stroke
		/// dynamic. It calculate the position of the joints according
		/// to the linear speed and delete useless joints regarding
		/// maximum length, maximum joint number and joint lifetime
		/// parameters.
		///
		/// \param time_interval Duration of the frame
		////////////////////////////////////////////////////////////
		void Calculate(float time_interval);
};
