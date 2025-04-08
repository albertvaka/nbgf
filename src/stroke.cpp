#ifdef _MSC_VER
#pragma warning(disable: 4244) //Cast int to float
#endif

#include "stroke.h"

#include "vec.h"
#include <SDL.h>
#include "window_drawraw.h"

float myRand(float low, float high)
{
	return static_cast<float>(std::rand()) / RAND_MAX * (high - low) + low;
}

Stroke::Joint::Joint() :
	birth_time(0),
	thickness_scale(1),
	angle(0),
	thickness(0)
{
}

Stroke::Stroke() :
	time(0),
	max_joints(0),
	max_length(0),
	min_segment_length(0),
	joint_lifetime(0),
	thickness(3),
	random_thickness(1, 1),
	start_thickness(1),
	end_thickness(1),
	stroke_offset(0),
	color_offset(0.f),
	start_sweep(0),
	end_sweep(0),
	start_sweep_style(SweepStyle::Soft),
	end_sweep_style(SweepStyle::Soft),
	stippling(0),
	shaking(0),
	loop(false),
	start_inner_color({ 255, 255, 255, 255 }),
	start_outer_color({ 255, 255, 255, 255 }),
	end_inner_color({ 255, 255, 255, 255 }),
	end_outer_color({ 255, 255, 255, 255 }),
	is_length_compiled(false),
	length(0),
	is_compiled(false)
{
}

float Stroke::GetLength()
{
	if (!is_length_compiled)
	{
		CompileLength();
	}
	return length;
}

bool Stroke::IsDrawable() const
{
	return joints.size() >= 2 && thickness != 0.f && stippling != 1.f;
}

void Stroke::SetJointLinearSpeed(float x, float y)
{
	SetJointLinearSpeed(vec(x, y));
}

void Stroke::SetJointLinearSpeed(const vec& joint_linear_speed)
{
	this->joint_linear_speed = joint_linear_speed;
}

void Stroke::SetMaxJoints(unsigned int max_joints)
{
	this->max_joints = max_joints;
}

void Stroke::SetMaxLength(float max_length)
{
	this->max_length = max_length;
}

void Stroke::SetMinSegmentLength(float min_segment_length)
{
	this->min_segment_length = min_segment_length;
}

void Stroke::SetJointLifetime(float joint_lifetime)
{
	this->joint_lifetime = joint_lifetime;
}

void Stroke::SetThickness(float thickness)
{
	this->thickness = thickness;
	is_compiled = false;
}

void Stroke::SetRandomThickness(float min, float max)
{
	random_thickness.x = min;
	random_thickness.y = max;
}

void Stroke::SetStartThickness(float start_thickness)
{
	SDL_assert(start_thickness >= 0.f && start_thickness <= 1.f);
	this->start_thickness = start_thickness;
	is_compiled = false;
}

void Stroke::SetEndThickness(float end_thickness)
{
	SDL_assert(end_thickness >= 0.f && end_thickness <= 1.f);
	this->end_thickness = end_thickness;
	is_compiled = false;
}

void Stroke::SetOffset(float stroke_offset)
{
	SDL_assert(stroke_offset >= -1.f && stroke_offset <= 1.f);
	this->stroke_offset = stroke_offset;
	is_compiled = false;
}

void Stroke::SetColorOffset(float color_offset)
{
	SDL_assert(color_offset >= 0.f);
	this->color_offset = color_offset;
	is_compiled = false;
}

void Stroke::SetSweep(float sweep)
{
	SetStartSweep(sweep);
	SetEndSweep(sweep);
	is_compiled = false;
}

void Stroke::SetStartSweep(float start_sweep)
{
	SDL_assert(start_sweep >= 0.f && start_sweep <= 1.f);
	this->start_sweep = start_sweep;
	is_compiled = false;
}

void Stroke::SetEndSweep(float end_sweep)
{
	SDL_assert(end_sweep >= 0.f && end_sweep <= 1.f);
	this->end_sweep = end_sweep;
	is_compiled = false;
}

void Stroke::SetSweepStyle(Stroke::SweepStyle sweep_style)
{
	this->start_sweep_style = sweep_style;
	this->end_sweep_style = sweep_style;
	is_compiled = false;
}

void Stroke::SetStartSweepStyle(Stroke::SweepStyle start_sweep_style)
{
	this->start_sweep_style = start_sweep_style;
	is_compiled = false;
}

void Stroke::SetEndSweepStyle(Stroke::SweepStyle end_sweep_style)
{
	this->end_sweep_style = end_sweep_style;
	is_compiled = false;
}

void Stroke::SetStippling(float stippling)
{
	SDL_assert(stippling >= 0.f && stippling <= 1.f);
	this->stippling = stippling;
}

void Stroke::SetShaking(float shaking)
{
	this->shaking = shaking;
}

void Stroke::SetLoop(bool loop)
{
	this->loop = loop;
	is_compiled = false;
}

void Stroke::SetInnerColor(const SDL_Color& inner_color)
{
	this->start_inner_color = inner_color;
	this->end_inner_color = inner_color;
	is_compiled = false;
}

void Stroke::SetOuterColor(const SDL_Color& outer_color)
{
	this->start_outer_color = outer_color;
	this->end_outer_color = outer_color;
	is_compiled = false;
}

void Stroke::SetStartInnerColor(const SDL_Color& start_inner_color)
{
	this->start_inner_color = start_inner_color;
	is_compiled = false;
}

void Stroke::SetStartOuterColor(const SDL_Color& start_outer_color)
{
	this->start_outer_color = start_outer_color;
	is_compiled = false;
}

void Stroke::SetEndInnerColor(const SDL_Color& end_inner_color)
{
	this->end_inner_color = end_inner_color;
	is_compiled = false;
}

void Stroke::SetEndOuterColor(const SDL_Color& end_outer_color)
{
	this->end_outer_color = end_outer_color;
	is_compiled = false;
}

bool Stroke::AddJoint(float x, float y, bool precompile)
{
	return AddJoint(vec(x, y), precompile);
}

bool Stroke::AddJoint(const vec& position, bool precompile)
{
	Joint joint;
	joint.position = position;
	joint.birth_time = time;
	joint.thickness_scale = myRand(random_thickness.x, random_thickness.y);

	if (joints.empty())
	{
		joints.push_back(joint);
		is_length_compiled = false;
		return true;
	}

	bool joint_added = false;

	// Adding joints
	if (min_segment_length)
	{
		float angle = joints[joints.size() - 1].position.AngleRads(position);
		vec last_point_position = joints[joints.size() - 1].position;
		int i = 0;
		while (position.Distance(joints[joints.size() - 1].position) >= min_segment_length)
		{
			
			joint.position = last_point_position + vec::FromAngleRads(angle, (i + 1) * min_segment_length);
			joint.thickness_scale = myRand(random_thickness.x, random_thickness.y);
			joints.push_back(joint);
			is_compiled = false;
			is_length_compiled = false;
			joint_added = true;
			++i;
		}
	}
	else
	{
		if (joints[joints.size() - 1].position != position)
		{
			joints.push_back(joint);
			is_compiled = false;
			is_length_compiled = false;
			joint_added = true;
		}
	}

	if (precompile && joints.size() >= 3)
	{
		Compile();
	}

	return joint_added;
}

bool Stroke::AddJoint(float x, float y, float thickness_scale, bool precompile)
{
	return AddJoint(vec(x, y), thickness_scale, precompile);
}

bool Stroke::AddJoint(const vec& position, float thickness_scale, bool precompile)
{
	Joint joint;
	joint.position = position;
	joint.birth_time = time;
	joint.thickness_scale = thickness_scale;

	if (joints.empty())
	{
		joints.push_back(joint);
		is_length_compiled = false;
		return true;
	}

	bool joint_added = false;

	// Adding joints
	if (min_segment_length)
	{
		float angle = joints[joints.size() - 1].position.AngleRads(position);
		vec last_point_position = joints[joints.size() - 1].position;
		int i = 0;
		while (position.Distance(joints[joints.size() - 1].position) >= min_segment_length)
		{
			joint.position = last_point_position + vec::FromAngleRads(angle, (i + 1) * min_segment_length);
			joints.push_back(joint);
			is_compiled = false;
			is_length_compiled = false;
			joint_added = true;
			++i;
		}
	}
	else
	{
		if (joints[joints.size() - 1].position.Distance(position) > 0.0001f)
		{
			joints.push_back(joint);
			is_compiled = false;
			is_length_compiled = false;
			joint_added = true;
		}
	}

	if (precompile) 
	{
		Compile();
	}

	return joint_added;
}

void Stroke::Clear()
{
	joints.clear();
}

void Stroke::Calculate(float time_interval)
{
	// Deleting unwanted points
	if (!joints.empty()) {
		/* Removing if
		- max joint counter is reached
		- max length is reached
		- points life is passed*/
		while ((joints.size() > max_joints && max_joints) || (GetLength() > max_length && max_length) || (time - joints[0].birth_time > joint_lifetime && joint_lifetime > 0))
		{
			joints.pop_front();
			is_compiled = false;
			is_length_compiled = false;
			if (joints.empty()) {
				break;
			}
		}
	}

	// Moving lefting points
	if (joint_linear_speed != vec(0.f, 0.f))
	{
		for (unsigned int i = 0; i < joints.size(); i++)
		{
			joints[i].position += joint_linear_speed * time_interval;
			joints[i].upper_point += joint_linear_speed * time_interval;
			joints[i].lower_point += joint_linear_speed * time_interval;
		}
	}

	time += time_interval;
}

void Stroke::Draw() const
{
	if (!is_compiled)
		if (!const_cast<Stroke*>(this)->Compile())
			return;

	if (shaking)
		const_cast<Stroke*>(this)->ComputeOffset();

	DrawSolid();
}

void Stroke::DrawExcluding(const Stroke& other) const
{
	if (!is_compiled)
		if (!const_cast<Stroke*>(this)->Compile())
			return;

	if (!other.is_compiled)
		if (!const_cast<Stroke*>(&other)->Compile())
			return;

	if (shaking)
		const_cast<Stroke*>(this)->ComputeOffset();

	if (other.shaking)
		const_cast<Stroke*>(this)->ComputeOffset();

	DrawSolidExcluding(other);
}

void Stroke::DrawSolid() const
{
	for (unsigned int i = 1; i < joints.size(); i++)
		DrawSolidBloc(i, i - 1);

	if (loop)
		DrawSolidBloc(0, joints.size() - 1);
}

void Stroke::DrawSolidExcluding(const Stroke& other) const
{
	for (unsigned int i = 1; i < joints.size(); i++)
		DrawSolidBlocExcluding(other, i, i - 1);

	if (loop)
		DrawSolidBlocExcluding(other, 0, joints.size() - 1);
}

void Stroke::DrawSolidBloc(unsigned int current_joint, unsigned int previous_joint) const
{
	vec center_previous = ((stippling) ? joints[previous_joint].position * (1.f - stippling / 2.f) + joints[current_joint].position * (stippling / 2.f) : joints[previous_joint].position) + joints[previous_joint].offset;
	vec center_next = ((stippling) ? joints[current_joint].position * (1.f - stippling / 2.f) + joints[previous_joint].position * (stippling / 2.f) : joints[current_joint].position) + joints[current_joint].offset;
	vec upper_previous = ((stippling) ? joints[previous_joint].upper_point * (1.f - stippling / 2.f) + joints[current_joint].upper_point * (stippling / 2.f) : joints[previous_joint].upper_point) + joints[previous_joint].offset;
	vec upper_next = ((stippling) ? joints[current_joint].upper_point * (1.f - stippling / 2.f) + joints[previous_joint].upper_point * (stippling / 2.f) : joints[current_joint].upper_point) + joints[current_joint].offset;
	vec lower_previous = ((stippling) ? joints[previous_joint].lower_point * (1.f - stippling / 2.f) + joints[current_joint].lower_point * (stippling / 2.f) : joints[previous_joint].lower_point) + joints[previous_joint].offset;
	vec lower_next = ((stippling) ? joints[current_joint].lower_point * (1.f - stippling / 2.f) + joints[previous_joint].lower_point * (stippling / 2.f) : joints[current_joint].lower_point) + joints[current_joint].offset;

	float first[] = {
		center_previous.x,
		center_previous.y,
		joints[previous_joint].inner_color.r / 255.f,
		joints[previous_joint].inner_color.g / 255.f,
		joints[previous_joint].inner_color.b / 255.f,
		joints[previous_joint].inner_color.a / 255.f,
		center_next.x,
		center_next.y,
		joints[current_joint].inner_color.r / 255.f,
		joints[current_joint].inner_color.g / 255.f,
		joints[current_joint].inner_color.b / 255.f,
		joints[current_joint].inner_color.a / 255.f,
		upper_previous.x,
		upper_previous.y,
		joints[previous_joint].outer_color.r / 255.f,
		joints[previous_joint].outer_color.g / 255.f,
		joints[previous_joint].outer_color.b / 255.f,
		joints[previous_joint].outer_color.a / 255.f,
		upper_next.x,
		upper_next.y,
		joints[current_joint].outer_color.r / 255.f,
		joints[current_joint].outer_color.g / 255.f,
		joints[current_joint].outer_color.b / 255.f,
		joints[current_joint].outer_color.a / 255.f

	};
	GPU_PrimitiveBatch(nullptr, Window::currentDrawTarget, GPU_TRIANGLE_STRIP, 4, first, 4, NULL, GPU_BATCH_XY_RGBA);

	float second[] = {
		center_previous.x,
		center_previous.y,
		joints[previous_joint].inner_color.r / 255.f,
		joints[previous_joint].inner_color.g / 255.f,
		joints[previous_joint].inner_color.b / 255.f,
		joints[previous_joint].inner_color.a / 255.f,
		center_next.x,
		center_next.y,
		joints[current_joint].inner_color.r / 255.f,
		joints[current_joint].inner_color.g / 255.f,
		joints[current_joint].inner_color.b / 255.f,
		joints[current_joint].inner_color.a / 255.f,
		lower_previous.x,
		lower_previous.y,
		joints[previous_joint].outer_color.r / 255.f,
		joints[previous_joint].outer_color.g / 255.f,
		joints[previous_joint].outer_color.b / 255.f,
		joints[previous_joint].outer_color.a / 255.f,
		lower_next.x,
		lower_next.y,
		joints[current_joint].outer_color.r / 255.f,
		joints[current_joint].outer_color.g / 255.f,
		joints[current_joint].outer_color.b / 255.f,
		joints[current_joint].outer_color.a / 255.f
	};
	GPU_PrimitiveBatch(nullptr, Window::currentDrawTarget, GPU_TRIANGLE_STRIP, 4, second, 4, NULL, GPU_BATCH_XY_RGBA);
}

void Stroke::DrawSolidBlocExcluding(const Stroke& other, unsigned int current_joint, unsigned int previous_joint) const
{
	vec upper_previous = ((stippling) ? joints[previous_joint].upper_point * (1.f - stippling / 2.f) + joints[current_joint].upper_point * (stippling / 2.f) : joints[previous_joint].upper_point) + joints[previous_joint].offset;
	vec upper_next = ((stippling) ? joints[current_joint].upper_point * (1.f - stippling / 2.f) + joints[previous_joint].upper_point * (stippling / 2.f) : joints[current_joint].upper_point) + joints[current_joint].offset;
	vec lower_previous = ((stippling) ? joints[previous_joint].lower_point * (1.f - stippling / 2.f) + joints[current_joint].lower_point * (stippling / 2.f) : joints[previous_joint].lower_point) + joints[previous_joint].offset;
	vec lower_next = ((stippling) ? joints[current_joint].lower_point * (1.f - stippling / 2.f) + joints[previous_joint].lower_point * (stippling / 2.f) : joints[current_joint].lower_point) + joints[current_joint].offset;

	vec other_upper_previous = ((other.stippling) ? other.joints[previous_joint].upper_point * (1.f - other.stippling / 2.f) + other.joints[current_joint].upper_point * (other.stippling / 2.f) : other.joints[previous_joint].upper_point) + other.joints[previous_joint].offset;
	vec other_upper_next = ((other.stippling) ? other.joints[current_joint].upper_point * (1.f - other.stippling / 2.f) + other.joints[previous_joint].upper_point * (other.stippling / 2.f) : other.joints[current_joint].upper_point) + other.joints[current_joint].offset;
	vec other_lower_previous = ((other.stippling) ? other.joints[previous_joint].lower_point * (1.f - other.stippling / 2.f) + other.joints[current_joint].lower_point * (other.stippling / 2.f) : other.joints[previous_joint].lower_point) + other.joints[previous_joint].offset;
	vec other_lower_next = ((other.stippling) ? other.joints[current_joint].lower_point * (1.f - other.stippling / 2.f) + other.joints[previous_joint].lower_point * (other.stippling / 2.f) : other.joints[current_joint].lower_point) + other.joints[current_joint].offset;

	float first[] = {
		other_upper_previous.x,
		other_upper_previous.y,
		joints[previous_joint].inner_color.r / 255.f,
		joints[previous_joint].inner_color.g / 255.f,
		joints[previous_joint].inner_color.b / 255.f,
		joints[previous_joint].inner_color.a / 255.f,
		other_upper_next.x,
		other_upper_next.y,
		joints[current_joint].inner_color.r / 255.f,
		joints[current_joint].inner_color.g / 255.f,
		joints[current_joint].inner_color.b / 255.f,
		joints[current_joint].inner_color.a / 255.f,
		upper_previous.x,
		upper_previous.y,
		joints[previous_joint].outer_color.r / 255.f,
		joints[previous_joint].outer_color.g / 255.f,
		joints[previous_joint].outer_color.b / 255.f,
		joints[previous_joint].outer_color.a / 255.f,
		upper_next.x,
		upper_next.y,
		joints[current_joint].outer_color.r / 255.f,
		joints[current_joint].outer_color.g / 255.f,
		joints[current_joint].outer_color.b / 255.f,
		joints[current_joint].outer_color.a / 255.f
	};
	GPU_PrimitiveBatch(nullptr, Window::currentDrawTarget, GPU_TRIANGLE_STRIP, 4, first, 4, NULL, GPU_BATCH_XY_RGBA);

	float second[] = {
		other_lower_previous.x,
		other_lower_previous.y,
		joints[previous_joint].inner_color.r / 255.f,
		joints[previous_joint].inner_color.g / 255.f,
		joints[previous_joint].inner_color.b / 255.f,
		joints[previous_joint].inner_color.a / 255.f,
		other_lower_next.x,
		other_lower_next.y,
		joints[current_joint].inner_color.r / 255.f,
		joints[current_joint].inner_color.g / 255.f,
		joints[current_joint].inner_color.b / 255.f,
		joints[current_joint].inner_color.a / 255.f,
		lower_previous.x,
		lower_previous.y,
		joints[previous_joint].outer_color.r / 255.f,
		joints[previous_joint].outer_color.g / 255.f,
		joints[previous_joint].outer_color.b / 255.f,
		joints[previous_joint].outer_color.a / 255.f,
		lower_next.x,
		lower_next.y,
		joints[current_joint].outer_color.r / 255.f,
		joints[current_joint].outer_color.g / 255.f,
		joints[current_joint].outer_color.b / 255.f,
		joints[current_joint].outer_color.a / 255.f
	};
	GPU_PrimitiveBatch(nullptr, Window::currentDrawTarget, GPU_TRIANGLE_STRIP, 4, second, 4, NULL, GPU_BATCH_XY_RGBA);
}

bool Stroke::Compile()
{
	if (joints.size() < 2)
		return false;

	float total_length = GetLength();

	// Determining joints' angles
	if (loop)
	{
		{
			// First joint
			joints[0].angle = joints[0].position.AngleDegs(joints[1].position);

			// Calculating in and out angles
			float angle1 = joints[joints.size() - 1].position.AngleDegs(joints[0].position);
			float angle2 = joints[0].position.AngleDegs(joints[1].position);

			// Calculating joint angle
			joints[0].angle = (angle1 + angle2) / 2.f;

			// In certain cases, previous line give the opposite of the wanted angle, applying correction if so
			if (Mates::Sign(angle1) != Mates::Sign(angle2) && ((angle1 > 90 || angle1 < -90) || (angle2 > 90 || angle2 < -90)) && (fabs(angle1) + fabs(angle2) > 180))
			{
				if (joints[0].angle >= 0)
					joints[0].angle -= 180;
				else
					joints[0].angle += 180;
			}
		}

		{
			// Last joint
			joints[joints.size() - 1].angle = joints[joints.size() - 2].position.AngleDegs(joints[joints.size() - 1].position);

			// Calculating in and out angles
			float angle1 = joints[joints.size() - 2].position.AngleDegs(joints[joints.size() - 1].position);
			float angle2 = joints[joints.size() - 1].position.AngleDegs(joints[0].position);

			// Calculating joint angle
			joints[joints.size() - 1].angle = (angle1 + angle2) / 2.f;

			// In certain cases, previous line give the opposite of the wanted angle, applying correction if so
			if (Mates::Sign(angle1) != Mates::Sign(angle2) && ((angle1 > 90 || angle1 < -90) || (angle2 > 90 || angle2 < -90)) && (fabs(angle1) + fabs(angle2) > 180))
			{
				if (joints[joints.size() - 1].angle >= 0)
					joints[joints.size() - 1].angle -= 180;
				else
					joints[joints.size() - 1].angle += 180;
			}
		}
	}
	else
	{
		// First joint
		joints[0].angle = joints[0].position.AngleDegs(joints[1].position);

		// Last joint
		joints[joints.size() - 1].angle = joints[joints.size() - 2].position.AngleDegs(joints[joints.size() - 1].position);
	}

	// other joints
	for (unsigned int i = 1; i < joints.size() - 1; i++)
	{
		// Calculating in and out angles
		float angle1 = joints[i - 1].position.AngleDegs(joints[i].position);
		float angle2 = joints[i].position.AngleDegs(joints[i + 1].position);

		// Calculating joint angle
		joints[i].angle = (angle1 + angle2) / 2.f;

		// In certain cases, previous line give the opposite of the wanted angle, applying correction if so
		if (Mates::Sign(angle1) != Mates::Sign(angle2) && ((angle1 > 90 || angle1 < -90) || (angle2 > 90 || angle2 < -90)) && (fabs(angle1) + fabs(angle2) > 180))
		{
			if (joints[i].angle >= 0)
				joints[i].angle -= 180;
			else
				joints[i].angle += 180;
		}
	}

	// Compiling outer points, inner points, thickness and color of each joint
	float l = 0;
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		if (i > 0) 
		{
			l += joints[i].position.Distance(joints[i - 1].position);
		}

		float alpha = l / total_length;
		float off = (stroke_offset + 1.f) / 2.f;
		float t = (thickness / 2.f) * joints[i].thickness_scale;

		joints[i].thickness = ComputeThickness(alpha, off, t);
		joints[i].upper_point = joints[i].position + vec::FromAngleDegs(joints[i].angle + 90, joints[i].thickness);
		joints[i].lower_point = joints[i].position + vec::FromAngleDegs(joints[i].angle - 90, joints[i].thickness);
		if (color_offset > 0) {
			alpha = 1.0 - std::exp(-color_offset * (l / total_length));
		}
		joints[i].inner_color = ColorInterpolation(start_inner_color, end_inner_color, alpha);
		joints[i].outer_color = ColorInterpolation(start_outer_color, end_outer_color, alpha);
	}

	is_compiled = true;
	return true;
}

void Stroke::ComputeOffset()
{
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		float offset = myRand(-shaking, shaking);
		joints[i].offset = vec::FromAngleDegs(joints[i].angle + 90, offset);
	}
}

BoxBounds Stroke::ComputeBoundingBox()
{
	float Left = joints[0].position.x;
	float Right = joints[0].position.x;
	float Top = joints[0].position.y;
	float Bottom = joints[0].position.y;
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		if (joints[i].lower_point.x < Left)
			Left = joints[i].lower_point.x;
		if (joints[i].upper_point.x < Left)
			Left = joints[i].upper_point.x;

		if (joints[i].lower_point.y < Top)
			Top = joints[i].lower_point.y;
		if (joints[i].upper_point.y < Top)
			Top = joints[i].upper_point.y;

		if (joints[i].lower_point.x > Right)
			Right = joints[i].lower_point.x;
		if (joints[i].upper_point.x > Right)
			Right = joints[i].upper_point.x;

		if (joints[i].lower_point.y > Bottom)
			Bottom = joints[i].lower_point.y;
		if (joints[i].upper_point.y > Bottom)
			Bottom = joints[i].upper_point.y;
	}
	return BoxBounds(Left, Top, Right - Left, Bottom - Top);
}

void Stroke::CompileLength()
{
	float temp_length = 0;

	if (joints.size() > 1)
	{
		for (unsigned int i = 1; i < joints.size(); i++)
		{
			temp_length += joints[i].position.Distance(joints[i - 1].position);
		}
	}
	length = temp_length;
	is_length_compiled = true;
}

float Stroke::ComputeThickness(float alpha, float offset, float t)
{
	if (alpha <= offset)
	{
		float a = (alpha * (1.f - end_thickness)) / offset + end_thickness;
		if (end_sweep)
		{
			float b = ApplySweep(end_sweep_style, a);
			t *= a * (1.f - end_sweep) + b * end_sweep;
		}
		else
		{
			t *= a;
		}
	}
	else
	{
		float a = ((alpha - 1.f) * (1.f - start_thickness)) / (offset - 1.f) + start_thickness;
		if (start_sweep)
		{
			float b = ApplySweep(start_sweep_style, a);
			t *= a * (1.f - start_sweep) + b * start_sweep;
		}
		else
		{
			t *= a;
		}
	}
	return t;
}

float Stroke::ApplySweep(SweepStyle sweep_style, float a)
{
	switch (sweep_style)
	{
	case SweepStyle::Soft:
		return sin(a / 2.f * M_PI);
	case SweepStyle::Softer:
		return sin(sin(a / 2.f * M_PI) / 2.f * M_PI);
	case SweepStyle::Sharp:
		return sin(a / 2.f * M_PI) * sin(a / 2.f * M_PI);
	default:
		return a;
	}
}

inline SDL_Color Stroke::ColorInterpolation(const SDL_Color& color1, const SDL_Color& color2, float alpha)
{
	return SDL_Color(static_cast<Uint8>(static_cast<float>(color1.r) * alpha + static_cast<float>(color2.r) * (1.f - alpha)),
		static_cast<Uint8>(static_cast<float>(color1.g) * alpha + static_cast<float>(color2.g) * (1.f - alpha)),
		static_cast<Uint8>(static_cast<float>(color1.b) * alpha + static_cast<float>(color2.b) * (1.f - alpha)),
		static_cast<Uint8>(static_cast<float>(color1.a) * alpha + static_cast<float>(color2.a) * (1.f - alpha)));

}
